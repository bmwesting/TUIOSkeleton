include_directories(${TUIOSkeleton_SOURCE_DIR}/oscpack)

set(SRC
    TuioClient.cpp
	TuioServer.cpp
	TuioTime.cpp
)

add_library(TUIOServer ${SRC})