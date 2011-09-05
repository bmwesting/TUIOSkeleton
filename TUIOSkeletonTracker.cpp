/*
 * TUIOSkeleton: A TUIO server for OpenNI devices.
 * Provides hands tracking and skeleton joint tracking over specified port.
 *
 * TUIOSkeletonTracker.cpp
 *
 * Created: 08/29/2011
 * Author: Brandt Westing, TACC
 */

#include <XnCppWrapper.h>

#include "TuioServer.h"
#include "TuioTime.h"
#include "TuioCursor.h"

#include "TUIOSkeletonTracker.h"

TUIOSkeletonTracker::TUIOSkeletonTracker()
{
    port_ = 3333;
    targetIP_ = "127.0.0.1";
    
    TuioServer_ = new TUIO::TuioServer(targetIP_.c_Str(), port_);
    TUIO::TuioTime::initSession();
    
    leftID_ = -1;
    rightID_ = -1;
    
    threshold_ = 200;
}

TUIOSkeletonTracker::~TUIOSkeletonTracker()
{
    
}

void TUIOSkeletonTracker::update(XnSkeletonJointPosition* joints)
{
    //Calculate distances
    int distanceLeft, distanceRight;
    distanceLeft  = joints[0].position.Z - joints[1].position.Z;
    distanceRight = joints[2].position.Z - joints[3].position.Z;
    
    //if elbow - hand less than threshold, remove from cursors (not reaching)
    if(distanceLeft < threshold)
    {
        // check to make sure cursor is in list before removing
        if (leftCursor_ != NULL)
        {
            TuioServer_->removeTuioCursor(leftCursor_);
            leftCursor_ = NULL;
        }
    }
     if(distanceRight < threshold)
    {
        // check to make sure cursor is in list before removing
        if (rightCursor_ != NULL)
        {
            TuioServer_->removeTuioCursor(rightCursor_);
            rightCursor_ = NULL;
        }
    }
    
    //if elbow - hand greater than threshold, add cursor to list (reaching gesture)
    if(distanceLeft > threshold)
    {
        // cursor doesnt exist yet, add it
        if(leftCursor == NULL)
        {
            
        }
    }
    
    
    
    //commit Tuio
    
    
}