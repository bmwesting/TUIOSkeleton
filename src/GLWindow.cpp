#include "GLWindow.h"
#include "SkeletonMath.h"

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
    
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

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
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // camera positioned at center of device camera
    glLoadIdentity();
    gluLookAt(depthMD.XRes()/2, depthMD.YRes()/2, -10, depthMD.XRes()/2, depthMD.YRes()/2, 0, 0, -1, 0);

    // light source at position (0,0,-10)
    float lightPosition[4] = { 0, 0, -10, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	drawScene(sceneMD, depthMD);
    
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

// draw skeleton joints as spheres
void GLWindow::drawJoints(const unsigned int player)
{
    // Array of available joints
    const unsigned int nJoints = 15;
    XnSkeletonJoint joints[nJoints] = 
            {XN_SKEL_HEAD,
             XN_SKEL_NECK,
             XN_SKEL_RIGHT_SHOULDER,
             XN_SKEL_LEFT_SHOULDER,
             XN_SKEL_RIGHT_ELBOW,
             XN_SKEL_LEFT_ELBOW,
             XN_SKEL_RIGHT_HAND,
             XN_SKEL_LEFT_HAND,
             XN_SKEL_RIGHT_HIP,
             XN_SKEL_LEFT_HIP,
             XN_SKEL_RIGHT_KNEE,
             XN_SKEL_LEFT_KNEE,
             XN_SKEL_RIGHT_FOOT,
             XN_SKEL_LEFT_FOOT,
             XN_SKEL_TORSO };           

    // set up glu object
    GLUquadricObj* quadobj;
    quadobj = gluNewQuadric();
                 
    for (int i = 0; i < nJoints; i++)
    {
        XnSkeletonJointPosition tPos;
        sensor_->getUserGenerator()->GetSkeletonCap().GetSkeletonJointPosition(player, joints[i], tPos);
        
        if (tPos.fConfidence == 1.0)
        {
            XnPoint3D pt;
            pt = tPos.position;
            
            sensor_->getDepthGenerator()->ConvertRealWorldToProjective(1, &pt, &pt);
            
            glPushMatrix();
            glTranslated(pt.X, pt.Y, pt.Z);
            gluSphere(quadobj,20.,16.,16.);
            glPopMatrix();
        }
    }

    // delete used quadric
    gluDeleteQuadric(quadobj);

}

void GLWindow::drawLimb(const unsigned int player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2)
{

    const float PI = 3.141592653589;

    XnSkeletonJointPosition tPos[2];
    sensor_->getUserGenerator()->GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, tPos[0]);
    sensor_->getUserGenerator()->GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, tPos[1]);
    
    // only draw cylinder if we are confident of both endpoints
    if (tPos[0].fConfidence <= 0.5 || tPos[1].fConfidence <= 0.5)
        return;
    
    XnPoint3D pt[2];
    pt[0] = tPos[0].position;
    pt[1] = tPos[1].position;
    
    // convert coordinates to pixel coordinate values
    sensor_->getDepthGenerator()->ConvertRealWorldToProjective(2, pt, pt);
    
    // Rotate scene so that cylinder drawn between pt2 and pt1 has a proper z-axis orientation
    // see gluCylinder() documentation
    
    // skeletal points, p is the vector formed between them
    Vector a = Vector(pt[0].X, pt[0].Y, pt[0].Z);
    Vector b = Vector(pt[1].X, pt[1].Y, pt[1].Z);
    Vector p = b - a;
    
    //glu cylinder vector - default direction for cylinders to face in glu
    Vector z = Vector(0,0,1);
    
    // c is the axis of rotation about z
    Vector c = z.crossProduct(p);
    
    // get the angle of rotation in degrees
    float angle = 180/PI * acos((z.dotProduct(p)/p.magnitude()));
    
    glPushMatrix();
    
    // translate to pt2
    glTranslated(pt[0].X,pt[0].Y,pt[0].Z);
    glRotatef(angle, c.getPoint().x_, c.getPoint().y_, c.getPoint().z_);
    
    // set up glu object
    GLUquadricObj* quadobj;
    quadobj = gluNewQuadric();
    
    gluCylinder(quadobj, 10, 10, p.magnitude(), 10, 10);
    
    glPopMatrix();
    
    // delete used quadric
    gluDeleteQuadric(quadobj);
    
}

void GLWindow::drawScene(const xn::SceneMetaData& sceneMD, const xn::DepthMetaData& depthMD)
{
    
    // draw skeleton of all tracked users
    for(int i = 0; i < sensor_->getNOTrackedUsers(); i++)
    {
        glColor4f(0.2,0.4,0.9,1.0);
        drawJoints(sensor_->getUID(i));
        
        glColor4f(0.0,0.85,0.5,1.0);
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
