#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "GLWindow.h"
#include <QtGui>

class SkeletonTracker;

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:

        MainWindow(SkeletonTracker* tracker);
        
    private:

        GLWindow glWindow_;
        SkeletonTracker* tracker_;
};

#endif
