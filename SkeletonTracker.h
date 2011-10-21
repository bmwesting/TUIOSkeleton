// 
//  SkeletonTracker: Maintains the TUIO Server and OpenNI device interface.
//                   Has modes for simple hand tracking and arm vectorization.
                  
#ifndef _SKELETON_TRACKER_H
#define _SKELETON_TRACKER_H

#include <TuioServer.h>
#include <TuioCursor.h>

#include "SensorDevice.h"
#include "TouchServer.h"

enum
{
    HANDMODE,
    VECTORMODE
};

class SkeletonTracker
{
    public:
        SkeletonTracker();
        ~SkeletonTracker();
    
        void update();
        void calibration();
        void enterTrackingLoop();
        int initialize(const std::string& ip, const unsigned int port);
        inline void waitForDeviceUpdate() { sensor_->waitForDeviceUpdateOnUser(); }
        inline void setModeToVector() { mode_ = VECTORMODE; }
        inline void setSmoothing(const float sFactor) { sensor_->setSmoothing(sFactor); }
        
        inline void loadCalibrationData(std::string& filename) { loadCalibration_ = true;
                                                        calibrationFile_ = filename; }
        inline void saveCalibrationData(std::string& filename) { saveCalibration_ = true;
                                                        calibrationFile_ = filename;     }
        
        // Should we take into account the confidence of skeletal joints?
        inline void setConfidenceTrackingOff() { confidenceTracking_ = FALSE; }
        inline void setConfidenceTrackingOn() { confidenceTracking_ = TRUE; }
        
        // Threshold determines whether events are sent through TUIO
        inline void setThreshold(const unsigned int threshold) { threshold_ = threshold; }
        inline unsigned int getThreshold() { return threshold_; }
        
        SensorDevice* getSensorDevice(){ return sensor_; }
    
    private:
        void updateHands();
        void updateVectors();
    
        SensorDevice* sensor_;
        TouchServer* touchServer_;
        
        int trackedUser_;
        unsigned int mode_;
        unsigned int threshold_;
        bool confidenceTracking_;
        bool saveCalibration_;
        bool loadCalibration_;
        std::string calibrationFile_;
};

#endif