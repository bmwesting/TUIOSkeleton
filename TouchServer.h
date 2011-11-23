// 
//  Implements a TuioServer, with specific cursors for right and left hands.
//  

#ifndef _TOUCH_SERVER_H
#define _TOUCH_SERVER_H

#include <TuioServer.h>

#include <map>

namespace TUIO
{
    class TuioCursor;
}

struct CursorPair
{
    TUIO::TuioCursor* leftCursor_;
    TUIO::TuioCursor* rightCursor_;
};

class TouchServer : public TUIO::TuioServer
{
    public:
        TouchServer(const char* ip, const unsigned int port);
        ~TouchServer() {}
        
        inline void initFrame();
        
        void updateLeftCursor(const unsigned int user, const float xp, const float yp);
        void updateRightCursor(const unsigned int user, const float xp, const float yp);
        void removeLeftCursor(const unsigned int user);
        void removeRightCursor(const unsigned int user);
        
        // overidden methods
        void removeTuioCursor(TUIO::TuioCursor *tcur);
        TUIO::TuioCursor* addTuioCursor(float xp, float yp);
        void updateTuioCursor(TUIO::TuioCursor *tcur, float xp, float yp);
        void commitFrame();
        
    private:
        std::map<unsigned int, CursorPair> cursorMap_;
        
        bool stateChanged_;
};

#endif