#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "GLWindow.h"
#include <QtGui>

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:

        MainWindow();

    private:

        GLWindow glWindow_;
};

#endif
