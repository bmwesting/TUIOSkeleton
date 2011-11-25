// 
//  SensorDevice: A wrapper for OpenNI devices

#ifndef _SENSOR_DEVICE_H
#define _SENSOR_DEVICE_H

#include <XnCppWrapper.h>
#include <string>
#include <vector>

#include "SkeletonMath.h"

class SensorDevice
{

    public:
        SensorDevice();
        ~SensorDevice();
        int initialize();
        
        inline void waitForDeviceUpdateOnUser() { context_.WaitOneUpdateAll(depthG_); }
        
        /*
         * Updates list of currently tracked users
         * Returns TRUE if there is a user who's skeleton is being tracked
         */
        bool isTracking();
        
        // stores the latest hand points in hands(preallocated):
        // hands[0] = left, hands[1] = right
        void getHandPoints(const unsigned int i, Point* const hands);
        
        // stores the latest elbow points in elbows
        // same convention as getHandPoints()
        void getElbowPoints(const unsigned int i, Point* const elbows);
        
        // stores the lastest arm points : hand, elbow, shoulder
        // 0 = l hand, 1 = r hand, 2 = left elbow....
        void getArmPoints(const unsigned int i, Point* const arms);
        
        // stores head points in externally managed array
        void getHeadPoint(const unsigned int i, Point* const head);
        
        // gets shoulder points
        void getShoulderPoints(const unsigned int i, Point* const shoulders);
        
        // returns unordered_map of points with keys of type <string>
        void getAllAvailablePoints(){}
        
        void getDepthMetaData(xn::DepthMetaData& depthMD) { depthG_.GetMetaData(depthMD); }
        void getDepthSceneMetaData(xn::SceneMetaData& sceneMD) { userG_.GetUserPixels(0, sceneMD); }
        
        void setPointModeToProjective() { pointModeProjective_ = true; }
        void setPointModeToReal() { pointModeProjective_ = false; }
        
        // set the smoothing factor
        inline void setSmoothing(const float smoothingF)
        {
            smoothingFactor_ = smoothingF;
        }
        
        int lookForCalibrationPoseOn();
        void startGeneratingAll() { context_.StartGeneratingAll(); }
        
        // get/set: calibration pose may be needed by OpenNI device, default FALSE
        inline bool getNeedCalibrationPose() const { return needCalibrationPose_; }
        inline void setNeedCalibrationPose(const bool b) { needCalibrationPose_ = b; }
        
        xn::UserGenerator* getUserGenerator() { return &userG_; }
        xn::DepthGenerator* getDepthGenerator() { return &depthG_; }
        
        unsigned int getNOTrackedUsers() { return trackedUsers_.size(); }
        unsigned int getUID(int i) { return trackedUsers_[i]; }
        void addTrackedUser(const int uID) { trackedUsers_.push_back(uID); }
        void removeTrackedUser(const int uID);
        
        void printAvailablePoses();
        
        void convertXnJointToPoint(XnSkeletonJointPosition* const j, Point* const p, unsigned int numPoints);
        
        inline const char* getPoseString() { return pose_.c_str(); }
        
        void saveCalibrationDataOn() { saveCalibration_ = true; }
        void saveCalibrationDataOn(std::string& filename) { saveCalibration_ = true;
                                                    calibrationFilename_ = filename; }
        bool getSaveCalibration() { return saveCalibration_; }
        void loadCalibrationFromFile(std::string& filename) { loadCalibration_ = true;
                                                    calibrationFilename_ = filename;
                                                    needCalibrationPose_ = false;       }
        
        const char* getCalibrationFilename() { return calibrationFilename_.c_str(); }
    
    private:
        xn::Context        context_;
        xn::DepthGenerator depthG_;
        xn::UserGenerator  userG_;
        
        bool pointModeProjective_;
        bool needCalibrationPose_;
        std::string pose_;
        
        std::vector<int> trackedUsers_;
        
        bool loadCalibration_;
        bool saveCalibration_;
        std::string calibrationFilename_;
        
        float smoothingFactor_;
        
};

#endif
