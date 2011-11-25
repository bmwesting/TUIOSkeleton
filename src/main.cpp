/*
 * TUIOSkeleton: A TUIO server for OpenNI devices.
 * Provides hands tracking and skeleton joint tracking over specified port.
 *
 * main.cpp
 *
 * Created: 08/29/2011
 * Updated: 11/26/2011
 * Author: Brandt Westing, TACC
 */


#include "SkeletonTracker.h"
#include "SensorDevice.h"
#include "main.h"

void printHelp()
{
    printf("Command line options: \n");
    printf("--help:             Print this message.\n");
    printf("--set-port:         Set TUIO UDP Port (default: 3333)\n");
    printf("--set-target-ip:    Set TUIO target IP (default: 127.0.0.1)\n");
    printf("--no-confidence:    Set Confidence Tracking off.\n");
    printf("--no-graphics:      Command line only (current default).\n");
    printf("--set-smoothing:    Set smoothing value (default: 0.8)\n");
    printf("--set-threshold:    Set threshold value (default: 400)\n");
    printf("--save-calibration: Save the calibrated skeleton after calibration\n");
    printf("--load-calibration: Load a calibration file when user detected\n");
    printf("--mode-vector:      Project cursors using head->hand vector\n");
}

int parseArgs(int nArgs, char** args, SkeletonTracker* tracker)
{

    g_port = 3333;
    g_ip = "127.0.0.1";
    noGUI = FALSE;
    
    for(int i = 1; i < nArgs; i++)
    {
    
        if (std::string(args[i]) == "--help" || std::string(args[i]) == "-h")
        {
            printHelp();
            return -1;
        }
        else if (std::string(args[i]) == "--set-port" || std::string(args[i]) == "-p")
        {
        
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            
            g_port = atoi(args[i+1]);
            
            if (g_port <= 0)
            {
                printf("UDP Port: %d is not a valid port.\n", g_port);
                return -1;
            }
            else
            {
                i++;
            }
        }
        else if (std::string(args[i]) == "--set-target-ip" || std::string(args[i]) == "-i")
        {
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            
            g_ip = args[i+1];
            i++;
        }
        else if (std::string(args[i]) == "--no-confidence" || std::string(args[i]) == "-nc")
        {
            tracker->setConfidenceTrackingOff();
        }
        else if (std::string(args[i]) == "--no-graphics" || std::string(args[i]) == "-ng")
        {
            g_noGUI = TRUE;
        }
        else if (std::string(args[i]) == "--set-smoothing" || std::string(args[i]) == "-s")
        {
        
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            
            tracker->setSmoothing(atof(args[i+1]));
            i++;
            
        }
        else if (std::string(args[i]) == "--set-threshold" || std::string(args[i]) == "-t")
        {
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            tracker->setThreshold(atoi(args[i+1]));
        }
        else if (std::string(args[i]) == "--save-calibration")
        {
            if( i + 1 > nArgs - 1)
                {
                    printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                    return -1;
                }
            std::string filename = std::string(args[i+1]);
            tracker->saveCalibrationData(filename);
            i++;
        }
        else if (std::string(args[i]) == "--load-calibration")
        {
            if( i + 1 > nArgs - 1)
            {
                printf("%s: invalid parameter.\n", std::string(args[i]).c_str());
                return -1;
            }
            std::string filename = std::string(args[i+1]);
            tracker->loadCalibrationData(filename);
            i++;
        }
        else if (std::string(args[i]) == "--mode-vector")
        {
            tracker->setModeToVector();
        }
        
        else
        {
            printf("'%s', not a valid argument.\n", args[i]);
            printHelp();
        }
    }
    
    return 0;

}

int main(int argc, char** argv)
{

    SkeletonTracker* tracker = new SkeletonTracker();
    
    if (parseArgs(argc, argv, tracker) == -1)
    {
        delete tracker;
        return -1;
    }
    
    // initializes the TuioServer and the attached sensor
    if (tracker->initialize(g_ip, g_port) == -1)
    {
        return -1;
    }
    
    // print number of available poses
    //tracker->getSensorDevice()->printAvailablePoses();
    
    tracker->calibration();
    
    // start generating data
    tracker->getSensorDevice()->startGeneratingAll();

    
    // bypass graphics and windowing
    if (g_noGUI)
        tracker->enterTrackingLoop();
    
    // do OpenGL drawing    
    else
    {
        g_app = new QApplication(argc, argv);
        g_mainWindow = new MainWindow(tracker);
        
        // enter event loop
        g_app->exec();
    }
    
    delete tracker;
        
    return 0;
}