/*
 * TUIOSkeleton: A TUIO server for OpenNI devices.
 * Provides hands tracking and skeleton joint tracking over specified port.
 *
 * TUIOSkeletonTracker.h
 *
 * Created: 08/29/2011
 * Author: Brandt Westing, TACC
 */

#IFNDEF TUIO_SKELETON_TRACKER_H_
#DEFINE TUIO_SKELETON_TRACKER_H_

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
        void update(XnSkeletonJointPosition* joints);
        
    private:
    
        unsigned int port_;
        std::string targetIP_;
        
        long leftID_;
        long rightID_;
        
        int threshold_;
        
        TUIO::TuioServer *TuioServer_;
    
};

#ENDIF