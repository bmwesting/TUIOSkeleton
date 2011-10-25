#include "MainWindow.h"

MainWindow::MainWindow()
{
    resize(800,600);

    setCentralWidget(&glWindow_);

    show();
}
