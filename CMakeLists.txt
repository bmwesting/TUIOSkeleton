project(TUIOSkeleton)

cmake_minimum_required(VERSION 2.8.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cpp")

# enable debugging, and show all warnings
set(CMAKE_CXX_FLAGS "-g -Wall")

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

set(SRCS
    main.cpp
    TUIOSkeletonTracker.cpp
)

add_executable(TUIOSkeleton ${SRCS})

target_link_libraries(TUIOSkeleton ${LIBS} TUIOServer oscpack)
