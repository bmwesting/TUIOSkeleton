#include "MainWindow.h"

MainWindow::MainWindow(SkeletonTracker* tracker)
{
    resize(640,480);

    setCentralWidget(&glWindow_);
    setWindowTitle("TUIOSkeleton Tracker");
    
    tracker_ = tracker;
    glWindow_.setTracker(tracker);
    
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(timerHandler()));
    timer_->start(0);
    
    show();
}
