#include "GLWindow.h"

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

void GLWindow::initializeGL()
{
    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // setup lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // black background
    glClearColor(0,0,0,0);
}

void GLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camera positioned at (0,0,-10) looking at (0,0,0) with up vector (0,1,0)
    glLoadIdentity();
    gluLookAt(0, 0, -10, 0, 0, 0, 0, 1, 0);

    // light source at position (0,0,-10)
    float lightPosition[4] = { 0, 0, -10, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    render();
}

void GLWindow::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / (float)height, 0.01, 1000.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    update();
}

void GLWindow::render()
{
    // render a sphere of radius 10 at position (0,0,-50)
    glPushMatrix();

    glTranslated(0, 0, 0);
    glColor4f(1,1,1,1);
    glutSolidSphere(1, 16, 16);

    glPopMatrix();
}
