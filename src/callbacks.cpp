#include <XnCppWrapper.h>
#include "SensorDevice.h"

//Callbacks for calibration events
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
    printf("Calibration started for user %d\n", nId);
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
    SensorDevice* sensor = (SensorDevice*) pCookie;
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        // Calibration succeeded
        printf("Calibration completed: Start tracking user %d\n", nId);
        sensor->getUserGenerator()->GetSkeletonCap().StartTracking(nId);
        
        //are we saving calibration data
        if(sensor->getSaveCalibration())
        {
            // Save user's calibration to file
			XnStatus rc = sensor->getUserGenerator()->GetSkeletonCap().SaveCalibrationDataToFile(nId, sensor->getCalibrationFilename());
			if (rc != XN_STATUS_OK)
			    printf("Unable to save calibration data to file: %s! Check permissions or give filename as argument.\n", sensor->getCalibrationFilename());
			else
			    printf("Saved calibration data to file: %s\n", sensor->getCalibrationFilename());
        }
    }
    else
    {
        // Calibration failed
        printf("Calibration failed for user %d\n", nId);
        if (sensor->getNeedCalibrationPose())
        {
            sensor->getUserGenerator()->GetPoseDetectionCap().StartPoseDetection(sensor->getPoseString(), nId);
        }
        else
        {
            sensor->getUserGenerator()->GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    SensorDevice* sensor = (SensorDevice*) pCookie;
    printf("New User %d\n", nId);
    // New user found
    if (sensor->getNeedCalibrationPose())
    {
        sensor->getUserGenerator()->GetPoseDetectionCap().StartPoseDetection(sensor->getPoseString(), nId);
    }
    
    //autocalibrate
    else
    {
        printf("Auto-calibrating user %d.\n", nId);
        sensor->getUserGenerator()->GetSkeletonCap().RequestCalibration(nId,TRUE);
    }
    
    /*
    // we are loading the calibration data from a file
    else
    {
        printf("Loading calibration data from file: %s for user: %d.\n", sensor->getCalibrationFilename(), nId);
        XnStatus rc = sensor->getUserGenerator()->GetSkeletonCap().LoadCalibrationDataFromFile(nId, sensor->getCalibrationFilename());
		if (rc == XN_STATUS_OK)
		{
			// Make sure state is coherent
			sensor->getUserGenerator()->GetPoseDetectionCap().StopPoseDetection(nId);
			sensor->getUserGenerator()->GetSkeletonCap().StartTracking(nId);
		}
    }
    */
}

// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    printf("Lost user %d\n", nId);
}

// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
    SensorDevice* sensor = (SensorDevice*) pCookie;
    printf("Pose %s detected for user %d\n", sensor->getPoseString(), nId);
    sensor->getUserGenerator()->GetPoseDetectionCap().StopPoseDetection(nId);
    sensor->getUserGenerator()->GetSkeletonCap().RequestCalibration(nId, TRUE);
}