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

# Create the executable
ADD_EXECUTABLE(MobotServer ${sources})

# Add linked libraries
TARGET_LINK_LIBRARIES(MobotServer ${CMAKE_THREAD_LIBS_INIT})

# Add Rewrite executable
ADD_EXECUTABLE(Rewrite ${sources})
TARGET_LINK_LIBRARIES(Rewrite ${CMAKE_THREAD_LIBS_INIT})
TARGET_COMPILE_DEFINITIONS(Rewrite PUBLIC REWRITE)