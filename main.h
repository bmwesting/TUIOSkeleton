#ifndef MAIN_H
#define MAIN_H

#include "MainWindow.h"
#include <cstring>

extern QApplication * g_app;
extern MainWindow * g_mainWindow;

extern std::string g_ip;
extern unsigned int g_port;
extern bool g_noGUI;

#endif