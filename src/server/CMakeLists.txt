CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(MobotServer)

# Add dependencies
FIND_PACKAGE(Threads REQUIRED)

SET(sources
    main.cpp
    TCPServer.cpp
    Device.cpp
    StackTrace.cpp

    Data.cpp

    data_processors.cpp
    data_writers.cpp
    utilities.cpp

    handler_client.h
)

ADD_EXECUTABLE(MobotServer ${sources})

# Add linked libraries
TARGET_LINK_LIBRARIES(MobotServer ${CMAKE_THREAD_LIBS_INIT})