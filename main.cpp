/*
 * TUIOSkeleton: A TUIO server for OpenNI devices.
 * Provides hands tracking and skeleton joint tracking over specified port.
 *
 * main.cpp
 *
 * Created: 08/29/2011
 * Author: Brandt Westing, TACC
 */

#include "define.h"

//Callbacks for calibration events
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
    printf("Calibration started for user %d\n", nId);
}

// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
    if (bSuccess)
    {
        // Calibration succeeded
        printf("Calibration complete, start tracking user %d\n", nId);
        g_userGenerator->GetSkeletonCap().StartTracking(nId);
        g_trackedUser = nId;
    }
    else
    {
        // Calibration failed
        printf("Calibration failed for user %d\n", nId);
        if (g_bNeedPose)
        {
            g_userGenerator->GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
        }
        else
        {
            g_userGenerator->GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        // Calibration succeeded
        printf("Calibration complete, start tracking user %d\n", nId);
        g_userGenerator->GetSkeletonCap().StartTracking(nId);
    }
    else
    {
        // Calibration failed
        printf("Calibration failed for user %d\n", nId);
        if (g_bNeedPose)
        {
            g_userGenerator->GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
        }
        else
        {
            g_userGenerator->GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}

// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    printf("New User %d\n", nId);
    // New user found
    if (g_bNeedPose)
    {
        g_userGenerator->GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
    }
    else
    {
        g_userGenerator->GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}

// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    printf("Lost user %d\n", nId);
}

// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
    printf("Pose %s detected for user %d\n", strPose, nId);
    g_userGenerator->GetPoseDetectionCap().StopPoseDetection(nId);
    g_userGenerator->GetSkeletonCap().RequestCalibration(nId, TRUE);
}


int initializeOpenNI()
{
    g_context        = new xn::Context();
    g_depthGenerator = new xn::DepthGenerator();
    g_userGenerator  = new xn::UserGenerator();
    g_context->Init();
    
    XnStatus rc = XN_STATUS_OK;
    xn::EnumerationErrors errors;
    xn::Query query;

    xn::NodeInfoList deviceList;
    rc = g_context->EnumerateProductionTrees(XN_NODE_TYPE_DEVICE, NULL, deviceList, NULL);
    
    if(rc != XN_STATUS_OK)
    {
        printf("No devices found!\n");
        return -1;
    }
    
    //Future: expand to support multiple devices
    xn::NodeInfoList::Iterator iter = deviceList.Begin();
    
    xn::NodeInfo node = (*iter);
    query.AddNeededNode(node.GetInstanceName());

    rc = g_context->CreateAnyProductionTree(XN_NODE_TYPE_DEPTH, &query, *g_depthGenerator);
    CHECK_RC(rc, "Find depth generator");
    rc = g_context->CreateAnyProductionTree(XN_NODE_TYPE_USER, &query, *g_userGenerator);
    CHECK_RC(rc, "Find user generator");
    
    
    if(TRUE == g_depthGenerator->IsCapabilitySupported("Mirror"))
    {
        printf("Image mirroring is supported by device.\n");
        rc = g_depthGenerator->GetMirrorCap().SetMirror(TRUE);
        CHECK_RC(rc, "Setting Image Mirroring");
    }
    
    return 0;
}

int main(int argc, char** argv)
{
    initializeOpenNI();
    
    XnStatus rc = XN_STATUS_OK;
    xn::DepthMetaData depthMD;
    g_depthGenerator->GetMetaData(depthMD);
    
    XnUserID users[20];
    XnUInt16 nUsers;
    
    XnSkeletonJointPosition joint;
    
    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
    
    g_userGenerator->RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
    g_userGenerator->GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
    g_userGenerator->GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);

    if (g_userGenerator->GetSkeletonCap().NeedPoseForCalibration())
    {
        g_bNeedPose = TRUE;
        if (!g_userGenerator->IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        {
            printf("Pose required, but not supported by device\n");
            return 1;
        }
        rc = g_userGenerator->GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
        CHECK_RC(rc, "Register to Pose Detected");
        g_userGenerator->GetSkeletonCap().GetCalibrationPose(g_strPose);
    }
    
    //g_userGenerator->GetSkeletonCap().RegisterToCalibrationInProgress(MyCalibrationInProgress, NULL, hCalibrationInProgress);
    //g_UserGenerator.GetPoseDetectionCap().RegisterToPoseInProgress(MyPoseInProgress, NULL, hPoseInProgress);
    
    g_userGenerator->GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    
    //TUIOTracker = new TUIOPointTracker(depthGenerator);
    //TUIOTracker->SetResolutionLimits(depthMD.XRes(), depthMD.YRes());
            
    g_context->StartGeneratingAll();
        
    while(1)
    {
        // Read next available data
        g_context->WaitOneUpdateAll(*g_userGenerator);
        //sleep(1);
        //printf("Number of users: %d\n", g_userGenerator->GetNumberOfUsers());
        //nUsers = g_userGenerator->GetNumberOfUsers();
        
        nUsers = g_userGenerator->GetNumberOfUsers();
        g_userGenerator->GetUsers(users, nUsers);
        //printf("Number of users: %d\n", nUsers);
        for(int i = 0; i < nUsers; i++)
        {
            //printf("users[%d] = %d\n", i, users[i]);
            if(g_userGenerator->GetSkeletonCap().IsTracking(users[i]))
            {
                g_userGenerator->GetSkeletonCap().GetSkeletonJointPosition(users[i], XN_SKEL_RIGHT_HAND, joint);
                printf("hand position: %f, %f, %f confidence: %f\n", joint.position.X, joint.position.Y, joint.position.Z, joint.fConfidence);
            }
        }
    }
        
    g_context->Shutdown();
    
    return 0;
}