project(TUIOSkeleton)

cmake_minimum_required(VERSION 2.4.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cpp")

# enable debugging, remove Mac deprecation warnings
set(CMAKE_CXX_FLAGS "-g -Wno-unknown-pragmas -Wno-deprecated -Wno-deprecated-declarations")

# TUIO Library Directories
add_subdirectory(TUIO)
add_subdirectory(oscpack)

# Use TUIO and OSC Libraries
include_directories(${TUIOSkeleton_SOURCE_DIR}/TUIO)
include_directories(${TUIOSkeleton_SOURCE_DIR}/oscpack)
link_directories(${TUIOSkeleton_BINARY_DIR}/TUIO)
link_directories(${TUIOSkeleton_BINARY_DIR}/oscpack)

# OpenNI
include_directories("/usr/include/ni")
set(LIBS ${LIBS} OpenNI)

# find and setup Qt4
# see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for details
find_package(Qt4 REQUIRED) 
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})
set(LIBS ${LIBS} ${QT_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLUT_LIBRARIES})


set(SRCS
    src/main.cpp
    src/SkeletonTracker.cpp
    src/SensorDevice.cpp
    src/TouchServer.cpp
    src/callbacks.cpp
    src/SkeletonMath.cpp
    src/main.cpp
    src/GLWindow.cpp
    src/MainWindow.cpp
)

set(MOC_HEADERS
    src/MainWindow.h
)

qt4_wrap_cpp(MOC_OUTFILES ${MOC_HEADERS})

add_executable(TUIOSkeleton ${SRCS} ${MOC_OUTFILES})

target_link_libraries(TUIOSkeleton ${LIBS} TUIOServer oscpack)
