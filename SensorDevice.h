// 
//  SensorDevice: A wrapper for OpenNI devices

#ifndef _SENSOR_DEVICE_H
#define _SENSOR_DEVICE_H

#include <XnCppWrapper.h>
#include <string>

struct Point
{
    float x_,y_,z_,confidence_;
};

class SensorDevice
{

    public:
        SensorDevice();
        ~SensorDevice();
        int initialize();
        
        inline void waitForDeviceUpdateOnUser() { context_.WaitOneUpdateAll(depthG_); }
        
        // returns TRUE if the device is currently tracking a user
        bool isTracking();
        
        // stores the latest hand points in hands(preallocated):
        // hands[0] = left, hands[1] = right
        void getHandPoints(Point* const hands);
        
        // stores the latest elbow points in elbows
        // same convention as getHandPoints()
        void getElbowPoints(Point* const elbows);
        
        // stores the lastest arm points : hand, elbow, shoulder
        // 0 = l hand, 1 = r hand, 2 = left elbow....
        void getArmPoints(Point* const arms);
        
        // stores head points in externally managed array
        void getHeadPoint(Point* const head);
        
        // gets shoulder points
        void getShoulderPoints(Point* const shoulders);
        
        // returns unordered_map of points with keys of type <string>
        void getAllAvailablePoints(){}
        
        // set the smoothing factor
        inline void setSmoothing(const float smoothingF)
        {
            userG_.GetSkeletonCap().SetSmoothing(smoothingF);
        }
        
        int lookForCalibrationPoseOn();
        void startGeneratingAll() { context_.StartGeneratingAll(); }
        
        // get/set: calibration pose may be needed by OpenNI device, default FALSE
        inline bool getNeedCalibrationPose() const { return needCalibrationPose_; }
        inline void setNeedCalibrationPose(const bool b) { needCalibrationPose_ = b; }
        
        xn::UserGenerator* getUserGenerator() { return &userG_; }
        
        void setTrackedUser(const int uID) { trackedUser_ = uID; }
        
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
        
        int trackedUser_;
        
        bool loadCalibration_;
        bool saveCalibration_;
        std::string calibrationFilename_;
        
};

#endif
