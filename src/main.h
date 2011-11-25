#ifndef MAIN_H
#define MAIN_H

#include "MainWindow.h"
#include <cstring>

QApplication * g_app;
MainWindow * g_mainWindow;

std::string g_ip;
unsigned int g_port;
bool g_noGUI;

#endif