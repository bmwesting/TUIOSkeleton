/*
 * TUIOSkeleton: A TUIO server for OpenNI devices.
 * Provides hands tracking and skeleton joint tracking over specified port.
 *
 * TUIOSkeletonTracker.cpp
 *
 * Created: 08/29/2011
 * Author: Brandt Westing, TACC
 */

// Temporary fix for incorrect projective coordinate bug with OpenNI
#define NORMALIZEX(x) (x + 180)/970 
#define NORMALIZEY(y) -1 * (y + 135)/(-735)

#include <XnCppWrapper.h>

#include "TuioServer.h"
#include "TuioTime.h"
#include "TuioCursor.h"

#include "TUIOSkeletonTracker.h" 

TUIOSkeletonTracker::TUIOSkeletonTracker()
{
    port_ = 3333;
    targetIP_ = "127.0.0.1";
    
    TuioServer_ = new TUIO::TuioServer(targetIP_.c_str(), port_);
    TUIO::TuioTime::initSession();
    
    leftCursor_ = NULL;
    rightCursor_ = NULL;
    
    xMax_ = 640.0;
    yMax_ = 480.0;
    
    threshold_ = 200;
    
    if(!TuioServer_->isConnected())
    {
        printf("Tuio Server unable to connect.\n");
    }
}

TUIOSkeletonTracker::~TUIOSkeletonTracker()
{
    //delete TuioServer_;
}

void TUIOSkeletonTracker::update(XnVector3D* joints, XnConfidence* conf)
{

    //printf("Number of cursors: %d\n", TuioServer_->getTuioCursors().size());
    // Do we send an update to the TuioServer?
    bool stateChanged = FALSE;

    TUIO::TuioTime time = TUIO::TuioTime::getSessionTime();
    //TuioServer_->initFrame(time);

    //Calculate distances
    int distanceLeft, distanceRight;
    distanceLeft  = joints[0].Z - joints[1].Z;
    distanceRight = joints[2].Z - joints[3].Z;
        
    //if elbow - hand less than threshold, remove from cursors (not reaching)
    if(distanceLeft < threshold_)
    {
        // check to make sure cursor is in list before removing
        if (leftCursor_ != NULL)
        {
            if(!stateChanged) TuioServer_->initFrame(time);
            
            TuioServer_->removeTuioCursor(leftCursor_);
            leftCursor_ = NULL;
            stateChanged = TRUE;
        }
    }
    if(distanceRight < threshold_)
    {
        // check to make sure cursor is in list before removing
        if (rightCursor_ != NULL)
        {
            if(!stateChanged) TuioServer_->initFrame(time);

            TuioServer_->removeTuioCursor(rightCursor_);
            rightCursor_ = NULL;
            stateChanged = TRUE;
        }
    }
    
    //if elbow - hand greater than threshold, add cursor to list (reaching gesture)
    if(distanceLeft >= threshold_)
    {
        // cursor doesnt exist yet, add it
        if(leftCursor_ == NULL)
        {
            if(!stateChanged) TuioServer_->initFrame(time);

            leftCursor_ = TuioServer_->addTuioCursor(NORMALIZEX(joints[1].X), NORMALIZEY(joints[1].Y));
            stateChanged = TRUE;
        }
        
        // update existing cursor
        else
        {
            if(!stateChanged) TuioServer_->initFrame(time);

            TuioServer_->updateTuioCursor(leftCursor_, NORMALIZEX(joints[1].X), NORMALIZEY(joints[1].Y));
            //printf("updated left cursor at %f,%f\n",NORMALIZEX(joints[1].X), NORMALIZEY(joints[1].Y));
            stateChanged = TRUE;

        }
    }
    if(distanceRight >= threshold_)
    {
        // cursor doesnt exist yet, add it
        if(rightCursor_ == NULL)
        {
            if(!stateChanged) TuioServer_->initFrame(time);
            
            rightCursor_ = TuioServer_->addTuioCursor(NORMALIZEX(joints[3].X), NORMALIZEY(joints[3].Y));
            stateChanged = TRUE;

        }
        
        // update existing cursor
        else
        {
            if(!stateChanged) TuioServer_->initFrame(time);
            
            TuioServer_->updateTuioCursor(rightCursor_, NORMALIZEX(joints[3].X), NORMALIZEY(joints[3].Y));
            //printf("updated right cursor at %f,%f\n",NORMALIZEX(joints[3].X), NORMALIZEY(joints[3].Y));
            stateChanged = TRUE;

        }
    }
    
    
    // Commit Tuio changes
    if(stateChanged)
    {
        TuioServer_->commitFrame();    
    }
    
}

void TUIOSkeletonTracker::setResolutionLimits(int x, int y)
{
    xMax_ = x,
    yMax_ = y;
}