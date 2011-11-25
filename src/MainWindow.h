#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "GLWindow.h"
#include <QtGui>

class SkeletonTracker;

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:

        MainWindow(SkeletonTracker* tracker);
        
    public slots:
        void timerHandler() { glWindow_.updateGL(); }
        
    private:
        QTimer* timer_;
        GLWindow glWindow_;
        SkeletonTracker* tracker_;
};

#endif
