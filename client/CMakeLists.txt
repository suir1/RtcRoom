cmake_minimum_required(VERSION 3.16)

project(RtcRoomClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network WebSockets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(USE_WEBRTC "Link against WebRTC libraries" OFF)

if (USE_WEBRTC)
    add_compile_definitions(USE_WEBRTC)
    # WebRTC Integration Instructions
    # Download WebRTC precompiled binaries for Windows (e.g., from webrtc.org or authorized builds)
    # and set WEBRTC_ROOT environment variable or CMake variable.
    # For this skeletal project, we assume headers are in include/ and libs in lib/
    # set(WEBRTC_ROOT "path/to/webrtc")
    # include_directories(${WEBRTC_ROOT}/include)
    # link_directories(${WEBRTC_ROOT}/lib)
endif()

add_executable(RtcRoomClient
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/SignalManager.cpp
    src/SignalManager.h
    src/WebRTCManager.cpp
    src/WebRTCManager.h
    src/VideoRenderer.cpp
    src/VideoRenderer.h
)

target_link_libraries(RtcRoomClient PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
)

if (USE_WEBRTC)
    # target_link_libraries(RtcRoomClient PRIVATE webrtc) # Uncomment when libs are present
    if (WIN32)
        # add_compile_definitions(WEBRTC_WIN)
    endif()
endif()
