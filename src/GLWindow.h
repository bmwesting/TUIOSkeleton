#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#include <QGLWidget>

namespace xn
{
    class SceneMetaData;
    class DepthMetaData;
}

class SkeletonTracker;
class SensorDevice;

class GLWindow : public QGLWidget
{
    public:
        
        void setTracker(SkeletonTracker* tracker) { tracker_ = tracker;
                                                    sensor_ = tracker->getSensorDevice(); }
        
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);
        //void render();
        void drawDepthMap(const xn::SceneMetaData& sceneMD, const xn::DepthMetaData& depthMD);
        
    private:
        SkeletonTracker* tracker_;
        SensorDevice* sensor_;
};

#endif
