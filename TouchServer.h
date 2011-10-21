// 
//  Implements a TuioServer, with specific cursors for right and left hands.
//  

#ifndef _TOUCH_SERVER_H
#define _TOUCH_SERVER_H

#include <TuioServer.h>

namespace TUIO
{
    class TuioCursor;
}

class TouchServer : public TUIO::TuioServer
{
    public:
        TouchServer(const char* ip, const unsigned int port);
        ~TouchServer() {}
        
        inline void initFrame();
        
        void updateLeftCursor(const float xp, const float yp);
        void updateRightCursor(const float xp, const float yp);
        void removeLeftCursor();
        void removeRightCursor();
        
        // overidden methods
        void removeTuioCursor(TUIO::TuioCursor *tcur);
        TUIO::TuioCursor* addTuioCursor(float xp, float yp);
        void updateTuioCursor(TUIO::TuioCursor *tcur, float xp, float yp);
        void commitFrame();
        
    private:
        TUIO::TuioCursor* leftCursor_;
        TUIO::TuioCursor* rightCursor_;
        
        bool stateChanged_;
};

#endif