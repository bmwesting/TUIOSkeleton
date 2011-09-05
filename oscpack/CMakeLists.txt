include_directories(${TUIOSkeleton_SOURCE_DIR}/oscpack)

set(SRC
    osc/OscOutboundPacketStream.cpp
	osc/OscPrintReceivedElements.cpp
	osc/OscReceivedElements.cpp
	osc/OscTypes.cpp
)

# Use windows networking if windows
if(WIN32)
	set(SRC ${SRC}
		ip/win32/UdpSocket.cpp
		ip/win32/NetworkingUtils.cpp
		)
endif(WIN32)

if(UNIX)
	set(SRC ${SRC}
		ip/posix/UdpSocket.cpp
		ip/posix/NetworkingUtils.cpp
		)
endif(UNIX)

add_library(oscpack ${SRC})