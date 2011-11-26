#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#include <QGLWidget>

#include "XnCppWrapper.h"

#include "SkeletonTracker.h"
#include "SensorDevice.h"

class GLWindow : public QGLWidget
{
    public:   
        void setTracker(SkeletonTracker* tracker) { tracker_ = tracker;
                                                    sensor_ = tracker->getSensorDevice(); }
        
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);
        
    private:
        SkeletonTracker* tracker_;
        SensorDevice* sensor_;
               
        void drawScene(const xn::SceneMetaData& sceneMD, const xn::DepthMetaData& depthMD);
        void drawLimb(const unsigned int player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2);
        void drawJoints(const unsigned int player);
};

#endif
