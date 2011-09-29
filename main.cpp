/*
 * TUIOSkeleton: A TUIO server for OpenNI devices.
 * Provides hands tracking and skeleton joint tracking over specified port.
 *
 * main.cpp
 *
 * Created: 08/29/2011
 * Author: Brandt Westing, TACC
 */

#include <cstring>

#include "define.h"
#include "TUIOSkeletonTracker.h"

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

void printHelp()
{
    printf("Command line options: \n");
    printf("--help:            Print this message.\n");
    printf("--set-port:        Set TUIO UDP Port (default: 3333)\n");
    printf("--set-target-ip:   Set TUIO target IP (default: 127.0.0.1)\n");
    printf("--no-confidence:   Set Confidence Tracking off.\n");
    printf("--no-graphics:     Command line only.\n");
    printf("--set-smoothing:   Set smoothing value (default: 0.1)\n");
    printf("--set-threshold:   Set threshold value (default: 200)\n");

}

int parseArgs(int nArgs, char** args, TUIOSkeletonTracker* tracker)
{
    for(int i = 1; i < nArgs; i++)
    {
    
        if (std::string(args[i]) == "--help" || std::string(args[i]) == "-h")
        {
            printHelp();
            return -1;
        }
        else if (std::string(args[i]) == "--set-port" || std::string(args[i]) == "-p")
        {
        
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            
            int port = atoi(args[i+1]);
            
            if (port <= 0)
            {
                printf("UDP Port: %d is not a valid port.\n", port);
                return -1;
            }
            else
            {
                tracker->setPort(port);
                i++;
            }
        }
        else if (std::string(args[i]) == "--set-target-ip" || std::string(args[i]) == "-i")
        {
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            
            const std::string IP(args[i+1]);
            tracker->setTargetIP(IP);
            i++;
        }
        else if (std::string(args[i]) == "--no-confidence" || std::string(args[i]) == "-nc")
        {
            tracker->setConfidenceTrackingOff();
        }
        else if (std::string(args[i]) == "--no-graphics" || std::string(args[i]) == "-ng")
        {
        }
        else if (std::string(args[i]) == "--set-smoothing" || std::string(args[i]) == "-s")
        {
        
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            
            g_smoothing = atof(args[i+1]);
        }
        else if (std::string(args[i]) == "--set-threshold" || std::string(args[i]) == "-t")
        {
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            tracker->setThreshold(atoi(args[i+1]));
        }
    
    }
    
    return 0;

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
    
    TUIOSkeletonTracker* tracker = new TUIOSkeletonTracker();

    if (parseArgs(argc, argv, tracker) == -1)
    {
        //g_context->Shutdown();
        return -1;
    }
    
    // Sets up the TUIOServer on specified port and IP
    if(tracker->initialize() == -1)
    {
        delete tracker;
        //g_context->Shutdown();
        return -1;
    }
    
    if (initializeOpenNI() == -1)
    {
        g_context->Shutdown();
        return -1;
    }
        
    XnStatus rc = XN_STATUS_OK;
    xn::DepthMetaData depthMD;
    g_depthGenerator->GetMetaData(depthMD);
    
    XnUserID users[20];
    XnUInt16 nUsers;
    
    XnSkeletonJointPosition joint[4];
    XnPoint3D points[4];
    XnConfidence confidence[4];
    
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
    
    g_userGenerator->GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    g_userGenerator->GetSkeletonCap().SetSmoothing(g_smoothing);
    
    tracker->setResolutionLimits(depthMD.XRes(), depthMD.YRes());
    
    g_context->StartGeneratingAll();
        
    while(1)
    {
        // Read next available data
        g_context->WaitOneUpdateAll(*g_userGenerator);
        
        nUsers = g_userGenerator->GetNumberOfUsers();
        g_userGenerator->GetUsers(users, nUsers);
        
        for(int i = 0; i < nUsers; i++)
        {
            if(g_userGenerator->GetSkeletonCap().IsTracking(users[i]))
            {
                g_userGenerator->GetSkeletonCap().GetSkeletonJointPosition(users[i], XN_SKEL_LEFT_ELBOW, joint[0]);
                g_userGenerator->GetSkeletonCap().GetSkeletonJointPosition(users[i], XN_SKEL_LEFT_HAND, joint[1]);
                g_userGenerator->GetSkeletonCap().GetSkeletonJointPosition(users[i], XN_SKEL_RIGHT_ELBOW, joint[2]);
                g_userGenerator->GetSkeletonCap().GetSkeletonJointPosition(users[i], XN_SKEL_RIGHT_HAND, joint[3]);
                
                points[0] = joint[0].position;
                points[1] = joint[1].position;
                points[2] = joint[2].position;
                points[3] = joint[3].position;
                
                confidence[0] = joint[0].fConfidence;
                confidence[1] = joint[1].fConfidence;
                confidence[2] = joint[2].fConfidence;
                confidence[3] = joint[3].fConfidence;

                // Convert X, Y, Z positions to pixel coordinates
                g_depthGenerator->ConvertRealWorldToProjective(4, points, points);
                
                //if(confidence[1] > 0.5) printf("left hand -> %f,%f,%f\n", points[1].X, points[1].Y, points[1].Z);
                
                tracker->update(points, confidence);

            }
        }
    }
        
    g_context->Shutdown();
    
    return 0;
}