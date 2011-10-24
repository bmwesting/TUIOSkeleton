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
                              calibrationFile_("TUIOSkeleton-UserCalibrationData.bin"),
                              vectorCalibrationTop_(TRUE),
                              vectorCalibrationBot_(TRUE),
                              msgCalibrationTop_(FALSE),
                              msgCalibrationBot_(FALSE)
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
    Point joints[3];
    sensor_->getHandPoints(joints);
    sensor_->getHeadPoint(joints+2);
    
    
    
    bool confidenceLeft = true;
    bool confidenceRight = true;
    
    // if we are tracking confidence, reject joints that are not confident
    if (confidenceTracking_)
    {
        if (joints[0].confidence_ <= 0.5)
            confidenceLeft = false;
        if (joints[1].confidence_ <= 0.5)
            confidenceRight = false;
        if (joints[3].confidence_ <= 0.5)
            confidenceLeft = confidenceRight = false;
    }
    
    O_ks_ = SkeletonVector(-670, 820, 0);
    screenBBoxBottom_ = SkeletonVector(515, 195, 0);
    
    // have we calibrated yet?
    if(!vectorCalibrationTop_)
    {
        // check to see if we are in calibration pose for top
        
        //first time, print message
        if(!msgCalibrationTop_)
        {
            printf("Calibrate top left!\n");
            msgCalibrationTop_ = true;
        }
        //calibrateTop();
        return;
    }
    else if(!vectorCalibrationBot_)
    {
        // check for calibration pose for bottom
        
        //first time, print message
        if(!msgCalibrationBot_)
        {
            printf("Calibrate bottom right!\n");
            msgCalibrationBot_ = true;
        }
        //calibrateBot();
        return;
    }
    
    if (!confidenceLeft)
    {
        SkeletonVector p1_l, p2_l, p3_l, t0_l;
        p1_l = joints[2];
        p2_l = joints[0];
        
        t0_l = p2_l - p1_l;
        
        float scaleC_l = -(p1_l.getPoint().z_)/(t0_l.getPoint().z_);
        p3_l = p1_l + t0_l * scaleC_l;
        
        p3_l.print();
        
        float cursorX, cursorY;
        
        cursorX = (p3_l.getPoint().x_ - O_ks_.getPoint().x_)/(screenBBoxBottom_.getPoint().x_ - O_ks_.getPoint().x_);
        cursorY = -(p3_l.getPoint().y_ - O_ks_.getPoint().y_)/(O_ks_.getPoint().y_ - screenBBoxBottom_.getPoint().y_);
        
        printf("CursorX: %f, CursorY: %f\n", cursorX, cursorY);
    }
    
}