#include "TouchServer.h"

#include <TuioCursor.h>
#include <TuioTime.h>

TouchServer::TouchServer(const char* ip, const unsigned int port) : 
                                  TuioServer(ip, port),
                                  cursorMap_(),
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

void TouchServer::removeLeftCursor(const unsigned int user)
{
    // this user does not currently have any cursors
    if (cursorMap_.count(user) == 0)
        return;
    
    // user is in map
    // if the users' left cursor is not NULL, remove it and set NULL
    if(cursorMap_[user].leftCursor_ != NULL)
    {
        removeTuioCursor(cursorMap_[user].leftCursor_);
        cursorMap_[user].leftCursor_ = NULL;
    }
}

void TouchServer::removeRightCursor(const unsigned int user)
{
    // this user does not currently have any cursors
    if (cursorMap_.count(user) == 0)
        return;
    
    // user is in map
    // if the users' left cursor is not NULL, remove it and set NULL
    if(cursorMap_[user].rightCursor_ != NULL)
    {
        removeTuioCursor(cursorMap_[user].rightCursor_);
        cursorMap_[user].rightCursor_ = NULL;
    }
}

void TouchServer::updateLeftCursor(const unsigned int user, const float xp, const float yp)
{
    //printf("Left cursor at: %f, %f.\n", xp, yp);
    
    // user not in map, add user and add cursor
    if(cursorMap_.count(user) == 0)
    {
        CursorPair cpair;
        cpair.leftCursor_ = addTuioCursor(xp, yp);
        cpair.rightCursor_ = NULL;
        cursorMap_.insert(std::pair<unsigned int, CursorPair>(user, cpair));
    }
    
    // user is in map, update cursor
    if(cursorMap_[user].leftCursor_ != NULL)
        updateTuioCursor(cursorMap_[user].leftCursor_, xp, yp);
    else
        cursorMap_[user].leftCursor_ = addTuioCursor(xp, yp);  
}

void TouchServer::updateRightCursor(const unsigned int user, const float xp, const float yp)
{
    //printf("Right cursor at: %f, %f.\n", xp, yp);
    
    // user not in map, add user and add cursor
    if(cursorMap_.count(user) == 0)
    {
        CursorPair cpair;
        cpair.leftCursor_ = NULL;
        cpair.rightCursor_ = addTuioCursor(xp, yp);
        cursorMap_.insert(std::pair<unsigned int, CursorPair>(user, cpair));
    }
    
    // user is in map, update cursor
    if(cursorMap_[user].rightCursor_ != NULL)
        updateTuioCursor(cursorMap_[user].rightCursor_, xp, yp);
    else
        cursorMap_[user].rightCursor_ = addTuioCursor(xp, yp);  
}