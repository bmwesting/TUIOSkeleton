#include "SensorDevice.h"
#include "callbacks.h"
#include "SkeletonMath.h"

#include <string>

// Checks to make sure status is good, or prints error and returns error code
inline int CHECK_RC(const unsigned int rc, const char* const description)
{
    if (rc != XN_STATUS_OK)
    {
        printf("%s failed: %s\n", description, xnGetStatusString(rc));
        return -1;
    }
}

SensorDevice::SensorDevice() :  context_(),
                                userG_(),
                                pointModeProjective_(TRUE),
                                needCalibrationPose_(TRUE),
                                trackedUsers_(),
                                pose_("Psi"),
                                loadCalibration_(FALSE),
                                saveCalibration_(FALSE),
                                calibrationFilename_("TUIOSkeleton-UserCalibrationData.bin"),
                                smoothingFactor_(0.8)
{}

SensorDevice::~SensorDevice()
{
    context_.Shutdown();
}

int SensorDevice::initialize()
{
    context_.Init();
    
    XnStatus rc = XN_STATUS_OK;
    
    XnMapOutputMode mapMode;
    
    // create depth and user generators
    rc = depthG_.Create(context_);
	CHECK_RC(rc, "Create depth generator");
    rc = userG_.Create(context_);
	CHECK_RC(rc, "Create user generator");
	
	depthG_.GetMapOutputMode(mapMode);
	
	// for now, make output map the native res of Kinect sensor
	mapMode.nXRes = XN_VGA_X_RES;
	mapMode.nYRes = XN_VGA_Y_RES;
	mapMode.nFPS  = 30;
	
	depthG_.SetMapOutputMode(mapMode);

    // turn on device mirroring
    if(TRUE == depthG_.IsCapabilitySupported("Mirror"))
    {
        //printf("Image mirroring is supported by device.\n");
        rc = depthG_.GetMirrorCap().SetMirror(TRUE);
        CHECK_RC(rc, "Setting Image Mirroring on depthG");
    }
    
    // make sure the user points are reported from the POV of the depth generator
    userG_.GetAlternativeViewPointCap().SetViewPoint(depthG_);
    userG_.GetSkeletonCap().SetSmoothing(smoothingFactor_);
    
    return 0;
}

// converts the OpenNI positions to simple 3D points
void SensorDevice::convertXnJointToPoint(XnSkeletonJointPosition* const joints, Point* const points, unsigned int numPoints)
{

    XnPoint3D xpt;
    for(int i = 0; i < numPoints; i++)
    {
        xpt = joints[i].position;
        if(pointModeProjective_)
            depthG_.ConvertRealWorldToProjective(1, &xpt, &xpt);

        points[i].confidence_ = joints[i].fConfidence;
        points[i].x_ = xpt.X;
        points[i].y_ = xpt.Y;
        points[i].z_ = xpt.Z;
    }
}
        
void SensorDevice::getHandPoints(const unsigned int i, Point* const hands)
{
    XnSkeletonJointPosition joints[2];
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_LEFT_HAND, joints[0]);
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_RIGHT_HAND, joints[1]);
    
    convertXnJointToPoint(joints, hands, 2);
}

void SensorDevice::getElbowPoints(const unsigned int i, Point* const elbows)
{
    XnSkeletonJointPosition joints[2];
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_LEFT_ELBOW, joints[0]);
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_RIGHT_ELBOW, joints[1]);
    convertXnJointToPoint(joints, elbows, 2);

}

void SensorDevice::getArmPoints(const unsigned int i, Point* const arms)
{
    getHandPoints(i, arms);
    getElbowPoints(i, arms+2);
    
    XnSkeletonJointPosition joints[2];
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_LEFT_ELBOW, joints[4]);
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_RIGHT_ELBOW, joints[5]);
    convertXnJointToPoint(joints, arms, 2);
}

void SensorDevice::getShoulderPoints(const unsigned int i, Point* const shoulders)
{
    XnSkeletonJointPosition joints[2];
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_LEFT_SHOULDER, joints[0]);
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_RIGHT_SHOULDER, joints[1]);
    convertXnJointToPoint(joints, shoulders, 2);
}

void SensorDevice::getHeadPoint(const unsigned int i, Point* const head)
{
    XnSkeletonJointPosition joints;
    userG_.GetSkeletonCap().GetSkeletonJointPosition(trackedUsers_[i], XN_SKEL_HEAD, joints);
    convertXnJointToPoint(&joints, head, 1);
}

bool SensorDevice::isTracking()
{
    XnUserID users[64];
    XnUInt16 nUsers = userG_.GetNumberOfUsers();
    trackedUsers_.clear();
    
    userG_.GetUsers(users, nUsers);
        
    for(int i = 0; i < nUsers; i++)
    {
        if(userG_.GetSkeletonCap().IsTracking(users[i]))
        {
            trackedUsers_.push_back(users[i]);
        }
    }
    
    if (!trackedUsers_.empty())
        return TRUE;
    else
        return FALSE;
    
}

// set device to look for calibration pose and supply callback functions for user events
int SensorDevice::lookForCalibrationPoseOn()
{
    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
    XnStatus rc = XN_STATUS_OK;

    userG_.RegisterUserCallbacks(User_NewUser, User_LostUser, this, hUserCallbacks);
    userG_.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, this, hCalibrationStart);
    userG_.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, this, hCalibrationComplete);

    if (needCalibrationPose_)
    {
        if (!userG_.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        {
            printf("Pose required, but not supported by device\n");
            return -1;
        }
        rc = userG_.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, this, hPoseDetected);
        CHECK_RC(rc, "Register to Pose Detected");
        userG_.GetSkeletonCap().GetCalibrationPose((XnChar*) pose_.c_str());
    }
    
    userG_.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    
}

// print the number of poses that the connected device supports
// Kinect: 1 Pose: "Psi"
void SensorDevice::printAvailablePoses()
{
    XnUInt32 numPoses = userG_.GetPoseDetectionCap().GetNumberOfPoses();
    
    printf("Number of poses: %d.\n", numPoses);
}