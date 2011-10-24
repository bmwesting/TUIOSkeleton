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
    
    // calculate distances
    int distanceLeft, distanceRight;
    distanceLeft  = joints[2].z_ - joints[0].z_;
    distanceRight = joints[3].z_ - joints[1].z_;
    
    
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
        if (joints[2].confidence_ <= 0.5)
            confidenceLeft = confidenceRight = false;
    }
    
    float distanceLeft, distanceRight;
    distanceLeft = joints[2].z_ - joints[0].z_;
    distanceRight = joints[2].z_ - joints[1].z_;
    
    O_ks_ = SkeletonVector(-1000, 1850, 0);
    screenBBoxBottom_ = SkeletonVector(1100, 450, 0);
    
    /* 
     * We are not doing auto-calibration just yet:: future!
     *
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
    */
    
    // we are not confident or not 'clicking' - remove cursor if exists
    if (!confidenceLeft || distanceLeft < threshold_)
    {
        touchServer_->removeLeftCursor();
    }
    if (!confidenceRight || distanceRight < threshold_)
    {
        touchServer_->removeRightCursor();
    }
    
    // we are confident in vector position and in 'clicking' state
    if (confidenceLeft && distanceLeft >= threshold_)
    {
        /* calculate the intersection of the vector formed by the user's head and hand
         *
         * p1 = head; p2 = hand; t0 is vector p2 - p1; c*t0 scaled the vector to the display
         * Note: assumes screen is at z = 0, or that the sensor is located in same z plane
         *
         */
         
        SkeletonVector p1_l, p2_l, p3_l, t0_l;
        p1_l = joints[2];
        p2_l = joints[0];
        
        t0_l = p2_l - p1_l;
        
        // calculate scale factor scaleC by known Z of screen and hand vector
        float scaleC_l = -(p1_l.getPoint().z_)/(t0_l.getPoint().z_);
        
        // p3 is projected point on display
        p3_l = p1_l + t0_l * scaleC_l;
        
        //p3_l.print();
        
        // The TUIO cursors based on the projected vector, will be normalized 0.0 - 1.0
        float cursorX_l, cursorY_l;
        
        /*
         * Normalization to screen coordinates, where O_ks_ is vector from sensor to screen top-left
         * screenBBoxBottom is vector from sensor to screen bottom right
         */
         
        cursorX_l = (p3_l.getPoint().x_ - O_ks_.getPoint().x_)/(screenBBoxBottom_.getPoint().x_ - O_ks_.getPoint().x_);
        cursorY_l = -(p3_l.getPoint().y_ - O_ks_.getPoint().y_)/(O_ks_.getPoint().y_ - screenBBoxBottom_.getPoint().y_);
                
        // cursor is outside display bounding box
        if ( cursorX_l < 0.0 || cursorX_l > 1.0 || cursorY_l < 0.0 || cursorY_l > 1.0)
        {
            touchServer_->removeLeftCursor();
        }
        // cursor is inside display area
        else
            touchServer_->updateLeftCursor(cursorX_l, cursorY_l);        
    }
    if (confidenceRight && distanceRight >= threshold_)
    {
        
        SkeletonVector p1_r, p2_r, p3_r, t0_r;
        p1_r = joints[2];
        p2_r = joints[1];
        
        t0_r = p2_r - p1_r;
        
        // calculate scale factor scaleC by known Z of screen and hand vector
        float scaleC_r = -(p1_r.getPoint().z_)/(t0_r.getPoint().z_);
        
        // p3 is projected point on display
        p3_r = p1_r + t0_r * scaleC_r;
        
                
        // The TUIO cursors based on the projected vector, will be normalized 0.0 - 1.0
        float cursorX_r, cursorY_r;
        
        cursorX_r = (p3_r.getPoint().x_ - O_ks_.getPoint().x_)/(screenBBoxBottom_.getPoint().x_ - O_ks_.getPoint().x_);
        cursorY_r = -(p3_r.getPoint().y_ - O_ks_.getPoint().y_)/(O_ks_.getPoint().y_ - screenBBoxBottom_.getPoint().y_);
                
        // cursor is outside display bounding box
        if ( cursorX_r < 0.0 || cursorX_r > 1.0 || cursorY_r < 0.0 || cursorY_r > 1.0)
        {
            touchServer_->removeRightCursor();
        }
        // cursor is inside display area
        else
            touchServer_->updateRightCursor(cursorX_r, cursorY_r);        
    }
    
    touchServer_->commitFrame();    

    
}