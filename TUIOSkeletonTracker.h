/*
 * TUIOSkeleton: A TUIO server for OpenNI devices.
 * Provides hands tracking and skeleton joint tracking over specified port.
 *
 * TUIOSkeletonTracker.h
 *
 * Created: 08/29/2011
 * Author: Brandt Westing, TACC
 */

#ifndef TUIO_SKELETON_TRACKER_H_
#define TUIO_SKELETON_TRACKER_H_

#include <string>

class XnSkeletonJointPosition;

namespace TUIO
{
    class TuioServer;
    class TuioCursor;
}

class TUIOSkeletonTracker
{
    public:
    
        TUIOSkeletonTracker();
        ~TUIOSkeletonTracker();
        
        /*
         * joints[0] = left elbow
         * joints[1] = left hand
         * joints[2] = right elbow
         * joints[3] = right hand
         */
        void update(XnVector3D* joints, XnConfidence* confidence);
        void setResolutionLimits(int x, int y);
        
    private:
    
        unsigned int port_;
        std::string targetIP_;
        
        TUIO::TuioCursor* leftCursor_;
        TUIO::TuioCursor* rightCursor_;
        
        int threshold_;
        float xMax_, yMax_;
        
        TUIO::TuioServer *TuioServer_;
    
};

#endif