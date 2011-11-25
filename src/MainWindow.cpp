#include "MainWindow.h"

MainWindow::MainWindow(SkeletonTracker* tracker)
{
    resize(640,480);

    setCentralWidget(&glWindow_);
    setWindowTitle("TUIOSkeleton Tracker");
    
    tracker_ = tracker;
    glWindow_.setTracker(tracker);

    show();
}
