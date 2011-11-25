#include "GLWindow.h"

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

inline unsigned int getClosestPowerOfTwo(const unsigned int n)
{
	unsigned int m = 2;
	while(m < n) m<<=1;

	return m;
}

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
    
    // wait for new data
    sensor_->waitForDeviceUpdateOnUser();
    
    // updates tracked users and sends TUIO cursors if necessary
    tracker_->update();
    
    // -- now let's draw the scene
    
    xn::DepthMetaData depthMD;
    xn::SceneMetaData sceneMD;
    sensor_->getDepthMetaData(depthMD);
    sensor_->getDepthSceneMetaData(sceneMD);
    
    //Greg stuff
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // camera positioned at center of device camera
    glLoadIdentity();
    gluLookAt(depthMD.XRes()/2, depthMD.YRes()/2, -10, depthMD.XRes()/2, depthMD.YRes()/2, 0, 0, -1, 0);

    // light source at position (0,0,-10)
    float lightPosition[4] = { 0, 0, -10, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	drawScene(sceneMD, depthMD);
    
    //glutSwapBuffers();
    
    //static int i = 0;

    //render();
    
}

void GLWindow::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / (float)height, 0.01, 10000.);
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

void GLWindow::drawLimb(const unsigned int player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2)
{

	XnSkeletonJointPosition joint1, joint2;
	sensor_->getUserGenerator()->GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	sensor_->getUserGenerator()->GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, joint2);

	if (joint1.fConfidence < 0.5 || joint2.fConfidence < 0.5)
	{
		return;
	}
    
	XnPoint3D pt[2];
	pt[0] = joint1.position;
	pt[1] = joint2.position;

	sensor_->getDepthGenerator()->ConvertRealWorldToProjective(2, pt, pt);
	
	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslated(pt[0].X, pt[0].Y, pt[0].Z);
	glutSolidSphere(15,16,16);
	glPopMatrix();
	glPushMatrix();
	glTranslated(pt[1].X, pt[1].Y, pt[1].Z);
	glutSolidSphere(15,16,16);
	glPopMatrix();

}

void GLWindow::drawScene(const xn::SceneMetaData& sceneMD, const xn::DepthMetaData& depthMD)
{
    
    // draw skeleton of all tracked users
    for(int i = 0; i < sensor_->getNOTrackedUsers(); i++)
    {
        
        drawLimb(sensor_->getUID(i), XN_SKEL_HEAD, XN_SKEL_NECK);
        drawLimb(sensor_->getUID(i), XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
        drawLimb(sensor_->getUID(i), XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
        drawLimb(sensor_->getUID(i), XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);

        drawLimb(sensor_->getUID(i), XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
        drawLimb(sensor_->getUID(i), XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
        drawLimb(sensor_->getUID(i), XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);

        drawLimb(sensor_->getUID(i), XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO);
        drawLimb(sensor_->getUID(i), XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO);

        drawLimb(sensor_->getUID(i), XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
        drawLimb(sensor_->getUID(i), XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
        drawLimb(sensor_->getUID(i), XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);

        drawLimb(sensor_->getUID(i), XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
        drawLimb(sensor_->getUID(i), XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
        drawLimb(sensor_->getUID(i), XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);

        drawLimb(sensor_->getUID(i), XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP);

    }
}
