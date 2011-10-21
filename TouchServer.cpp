#include "TouchServer.h"

#include <TuioCursor.h>
#include <TuioTime.h>

TouchServer::TouchServer(const char* ip, const unsigned int port) : 
                                  TuioServer(ip, port),
                                  leftCursor_(NULL),
                                  rightCursor_(NULL),
                                  stateChanged_(false)
{}

TUIO::TuioCursor* TouchServer::addTuioCursor(float xp, float yp)
{
    if(!stateChanged_)
    {
        stateChanged_ = true;
        TUIO::TuioTime time = TUIO::TuioTime::getSessionTime();
        TuioServer::initFrame(time);
    }
    return TuioServer::addTuioCursor(xp, yp);
}

void TouchServer::removeTuioCursor(TUIO::TuioCursor *tcur)
{
    if(!stateChanged_)
    {
        stateChanged_ = true;
        TUIO::TuioTime time = TUIO::TuioTime::getSessionTime();
        TuioServer::initFrame(time);
    }
    TuioServer::removeTuioCursor(tcur);
}

void TouchServer::updateTuioCursor(TUIO::TuioCursor* tcur, float xp, float yp)
{
    if(!stateChanged_)
    {
        stateChanged_ = true;
        TUIO::TuioTime time = TUIO::TuioTime::getSessionTime();
        TuioServer::initFrame(time);
    }
    TuioServer::updateTuioCursor(tcur, xp, yp);
}

void TouchServer::commitFrame()
{
    if(stateChanged_)
    {
        stateChanged_ = false;
        TuioServer::commitFrame();
    }
}

void TouchServer::removeLeftCursor()
{
    //printf("Removing left cursor.\n");
    if(leftCursor_ != NULL)
    {
        removeTuioCursor(leftCursor_);
        leftCursor_ = NULL;
    }
}

void TouchServer::removeRightCursor()
{
    //printf("Removing right cursor.\n");
    if(rightCursor_ != NULL)
    {
        removeTuioCursor(rightCursor_);
        rightCursor_ = NULL;
    }
}

void TouchServer::updateLeftCursor(const float xp, const float yp)
{
    //printf("Left cursor at: %f, %f.\n", xp, yp);
    
    if(leftCursor_ != NULL)
        updateTuioCursor(leftCursor_, xp, yp);
    else
        leftCursor_ = addTuioCursor(xp, yp);  
}

void TouchServer::updateRightCursor(const float xp, const float yp)
{
    //printf("Right cursor at: %f, %f.\n", xp, yp);
    
    if(rightCursor_ != NULL)
        updateTuioCursor(rightCursor_, xp, yp);
    else
        rightCursor_ = addTuioCursor(xp, yp);  
}