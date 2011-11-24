#include "SkeletonTracker.h"
#include "SensorDevice.h"
#include "SkeletonMath.h"

#include <math.h>

// normalize the joint coordinates to 0.0 - 1.0 for TUIO, assuming camera set to VGA
inline float NORMALIZEX(const float x) { return x/640.0; }
inline float NORMALIZEY(const float y) { return y/480.0; }

SkeletonTracker::SkeletonTracker() 
                            : touchServer_(NULL),
                              sensor_(NULL),
                              mode_(HANDMODE),
                              confidenceTracking_(TRUE),
                              threshold_(400),
                              saveCalibration_(FALSE),
                              loadCalibration_(FALSE),
                              calibrationFile_("usercal.skel"),
                              vectorCalibrationTop_(TRUE),
                              vectorCalibrationBot_(TRUE)
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
// TODO: check for 'hand by waist' event
void SkeletonTracker::updateHands()
{
    for (int i = 0; i < sensor_->getNOTrackedUsers(); i++)
    {
    
        Point jointsReal[4];
        sensor_->setPointModeToReal();
        sensor_->getHandPoints(i, jointsReal);
        sensor_->getShoulderPoints(i, jointsReal+2);
        
        // calculate euclidean distances between hand and shoulder points
        // distanceLeft = sqrt((left_hand_x - shoulder_x)^2 + ... )
        int distanceLeft, distanceRight;
        distanceLeft =  sqrt(pow((jointsReal[2].x_ - jointsReal[0].x_), 2) +
                             pow((jointsReal[2].y_ - jointsReal[0].y_), 2) +
                             pow((jointsReal[2].z_ - jointsReal[0].z_), 2));
        distanceRight = sqrt(pow((jointsReal[3].x_ - jointsReal[1].x_), 2) +
                             pow((jointsReal[3].y_ - jointsReal[1].y_), 2) +
                             pow((jointsReal[3].z_ - jointsReal[1].z_), 2));
                                 
        //get projective (pixel) coordinates for remaining processing
        Point joints[4];
        sensor_->setPointModeToProjective();
        sensor_->getHandPoints(i, joints);
        sensor_->getShoulderPoints(i, joints+2);    
        
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
            touchServer_->removeLeftCursor(sensor_->getUID(i));
        }
        if(distanceRight < threshold_ || !confidenceRight || !rightWithinFOV)
        {
            touchServer_->removeRightCursor(sensor_->getUID(i));
        }
        
        //if elbow - hand greater than threshold, add cursor to list (reaching gesture)
        if(distanceLeft >= threshold_ && confidenceLeft && leftWithinFOV)
        {
            touchServer_->updateLeftCursor(sensor_->getUID(i), NORMALIZEX(joints[0].x_), NORMALIZEY(joints[0].y_));
        }
        
        if(distanceRight >= threshold_ && confidenceRight && rightWithinFOV)
        {
            touchServer_->updateRightCursor(sensor_->getUID(i), NORMALIZEX(joints[1].x_), NORMALIZEY(joints[1].y_));
        }
        
    }
    
    // commit any changes to TUIO state
    touchServer_->commitFrame();    

}

//TODO: Use euclidian distance and also specify depth threshold to avoid 'hand by waist' cursors
void SkeletonTracker::updateVectors()
{
    
    // iterate over all currently tracked users, sending TUIO events if needed
    for (int i = 0; i < sensor_->getNOTrackedUsers(); i++)
    {
    
        Point joints[3];
        sensor_->setPointModeToReal();
        sensor_->getHandPoints(i, joints);
        sensor_->getHeadPoint(i, joints+2);
            
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
        
        // calculate euclidean distances between hand and head points
        // distanceLeft = sqrt((left_hand_x - head_x)^2 + ... )
        int distanceLeft, distanceRight;
        distanceLeft =  sqrt(pow((joints[2].x_ - joints[0].x_), 2) +
                             pow((joints[2].y_ - joints[0].y_), 2) +
                             pow((joints[2].z_ - joints[0].z_), 2));
        distanceRight = sqrt(pow((joints[2].x_ - joints[1].x_), 2) +
                             pow((joints[2].y_ - joints[1].y_), 2) +
                             pow((joints[2].z_ - joints[1].z_), 2));
        
        O_ks_ = SkeletonVector(-3100, 2650, 0);
        screenBBoxBottom_ = SkeletonVector(2000, 0, 0);
        
        // we are not confident or not 'clicking' - remove cursor if exists
        if (!confidenceLeft || distanceLeft < threshold_)
        {
            touchServer_->removeLeftCursor(sensor_->getUID(i));
        }
        if (!confidenceRight || distanceRight < threshold_)
        {
            touchServer_->removeRightCursor(sensor_->getUID(i));
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
            
            p3_l.print();
            
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
                touchServer_->removeLeftCursor(sensor_->getUID(i));
            }
            // cursor is inside display area
            else
                touchServer_->updateLeftCursor(sensor_->getUID(i), cursorX_l, cursorY_l);        
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
            //p3_r = p1_r + t0_r * scaleC_r;
            
                    
            // The TUIO cursors based on the projected vector, will be normalized 0.0 - 1.0
            float cursorX_r, cursorY_r;
            
            cursorX_r = (p3_r.getPoint().x_ - O_ks_.getPoint().x_)/(screenBBoxBottom_.getPoint().x_ - O_ks_.getPoint().x_);
            cursorY_r = -(p3_r.getPoint().y_ - O_ks_.getPoint().y_)/(O_ks_.getPoint().y_ - screenBBoxBottom_.getPoint().y_);
                    
            // cursor is outside display bounding box
            if ( cursorX_r < 0.0 || cursorX_r > 1.0 || cursorY_r < 0.0 || cursorY_r > 1.0)
            {
                touchServer_->removeRightCursor(sensor_->getUID(i));
            }
            // cursor is inside display area
            else
                touchServer_->updateRightCursor(sensor_->getUID(i), cursorX_r, cursorY_r);        
        }
    
    }
    
    // commit any changes to TUIO state
    touchServer_->commitFrame();    

}