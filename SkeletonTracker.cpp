#include "SkeletonTracker.h"
#include "SensorDevice.h"
#include "SkeletonMath.h"

// normalize the joint coordinates to 0.0 - 1.0 for TUIO
inline float NORMALIZEX(const float x) { return x/640.0; }
inline float NORMALIZEY(const float y) { return y/480.0; }

SkeletonTracker::SkeletonTracker() 
                            : touchServer_(NULL),
                              sensor_(NULL),
                              trackedUser_(-1),
                              mode_(HANDMODE),
                              confidenceTracking_(TRUE),
                              threshold_(250),
                              saveCalibration_(FALSE),
                              loadCalibration_(FALSE),
                              calibrationFile_("TUIOSkeleton-UserCalibrationData.bin")
{
}

int SkeletonTracker::initialize(const std::string& ip, const unsigned int port)
{   
    sensor_ = new SensorDevice();
    
    // initialize sensor and connect to device
    if (sensor_->initialize() == -1)
    {
        delete sensor_;
        sensor_ = NULL;
        return -1;
    }
    
    if (mode_ == VECTORMODE)
        sensor_->setPointModeToReal();
    
    touchServer_ = new TouchServer(ip.c_str(), port);
    
    if(!touchServer_->isConnected())
    {
        delete touchServer_;
        delete sensor_;
        touchServer_ = NULL;
        sensor_ = NULL;
        printf("Can't create Tuio Server on IP: %s, port: %d.\n", ip.c_str(), port);
        return -1;
    }
    
    return 0;
}

SkeletonTracker::~SkeletonTracker()
{
    if(touchServer_ != NULL)
        delete touchServer_;
    if(sensor_ != NULL)
        delete sensor_;
}

void SkeletonTracker::calibration()
{
    if(saveCalibration_)
        sensor_->saveCalibrationDataOn(calibrationFile_);
    if(loadCalibration_)
        sensor_->loadCalibrationFromFile(calibrationFile_);
    sensor_->lookForCalibrationPoseOn();
}

void SkeletonTracker::enterTrackingLoop()
{
    sensor_->startGeneratingAll();
    while(1)
    {
        // Read next available data
        sensor_->waitForDeviceUpdateOnUser();
        
        update();
    }
}

void SkeletonTracker::update()
{
    if(sensor_->isTracking())
    {
        if (mode_ == HANDMODE)
            updateHands();
        else if (mode_ == VECTORMODE)
            updateVectors();
    }
}

// if we are doing hand tracking, update the positions and cursors
void SkeletonTracker::updateHands()
{
    Point joints[4];
    sensor_->getHandPoints(joints);
    sensor_->getShoulderPoints(joints+2);
    
    //printf("Left Hand: %f, %f, %f.\n", joints[0].x_, joints[0].y_, joints[0].z_);
    //printf("Right Hand: %f, %f, %f.\n", joints[1].x_, joints[1].y_, joints[1].z_);

    // calculate distances
    int distanceLeft, distanceRight;
    distanceLeft  = joints[2].z_ - joints[0].z_;
    distanceRight = joints[3].z_ - joints[1].z_;
    
    //printf("Right hand Z: %f, right rear Z: %f. Difference: %d\n", joints[1].z_, joints[3].z_, distanceRight);
    
    bool confidenceLeft = TRUE;
    bool confidenceRight = TRUE;
    
    // we are taking into account the confidence of the joints position
    if (confidenceTracking_)
    {
        if (joints[0].confidence_ <= 0.5 || joints[2].confidence_ <= 0.5) confidenceLeft = FALSE;
        if (joints[1].confidence_ <= 0.5 || joints[3].confidence_ <= 0.5) confidenceRight = FALSE;
    }
    
    // Points must lie in device FOV - outside points are error prone
    bool leftWithinFOV  = TRUE;
    bool rightWithinFOV = TRUE;
    // if point lies outside kinect FOV
    if (joints[0].x_ > 640 || joints[0].x_ < 0 || joints[0].y_ > 480 || joints[0].y_ < 0)
        leftWithinFOV = false;
    if (joints[1].x_ > 640 || joints[1].x_ < 0 || joints[1].y_ > 480 || joints[1].y_ < 0)
        rightWithinFOV = false;
    
    //if elbow - hand less than threshold, remove from cursors (not reaching)
    if(distanceLeft < threshold_ || !confidenceLeft || !leftWithinFOV)
    {
        touchServer_->removeLeftCursor();
    }
    if(distanceRight < threshold_ || !confidenceRight || !rightWithinFOV)
    {
        touchServer_->removeRightCursor();
    }
    
    //if elbow - hand greater than threshold, add cursor to list (reaching gesture)
    if(distanceLeft >= threshold_ && confidenceLeft && leftWithinFOV)
    {
        touchServer_->updateLeftCursor(NORMALIZEX(joints[0].x_), NORMALIZEY(joints[0].y_));
    }
    
    if(distanceRight >= threshold_ && confidenceRight && rightWithinFOV)
    {
        touchServer_->updateRightCursor(NORMALIZEX(joints[1].x_), NORMALIZEY(joints[1].y_));
    }
    
    touchServer_->commitFrame();    

}

void SkeletonTracker::updateVectors()
{

}