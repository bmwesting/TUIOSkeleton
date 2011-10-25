#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#include <QGLWidget>

class GLWindow : public QGLWidget
{
    public:

        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);
        void render();
};

#endif
