CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(MobotServer)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
MESSAGE("${CMAKE_MODULE_PATH}")

###############################################################################
# Configure package dependencies
###############################################################################

# Add pthread dependency
FIND_PACKAGE(Threads REQUIRED)

# Add OpenCV dependency
IF (APPLE)
  # Assume we've installed OpenCV via brew
  SET("OpenCV_DIR" "/usr/local/Cellar/opencv3/3.0.0/share/OpenCV")
ENDIF()
FIND_PACKAGE(OpenCV REQUIRED)

# Add OSMesa
FIND_PACKAGE(OSMesa REQUIRED)
INCLUDE_DIRECTORIES(${OSMESA_INCLUDE_DIR})

# Add CUDA
FIND_PACKAGE(CUDA REQUIRED)

if (WIN32)
  set(EXTRA_CXX_FLAGS "/DVERBOSE /D_CRT_SECURE_NO_WARNINGS ")
  list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;-O2;-DVERBOSE") 
endif()
if (UNIX)
  if (APPLE)
    set(EXTRA_CXX_FLAGS "-DVERBOSE -msse2 -g")
    list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;-O2;-DVERBOSE") 
  else()
    set(EXTRA_CXX_FLAGS "-DVERBOSE -msse2 -std=c++0x -g")
    list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;-O2;-DVERBOSE") 
  endif()
endif()

# Add Ceres
FIND_PACKAGE(Ceres REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/cuSIFT)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/readerwriterqueue)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/json)

# Add curlcpp
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/lib/curlcpp)
INCLUDE_DIRECTORIES(${CURLCPP_SOURCE_DIR}/include)

# Add cusift
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/lib/cuSIFT)
INCLUDE_DIRECTORIES(${cuSIFT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${cuSIFT_SOURCE_DIR}/extras)

###############################################################################
# Set sources
###############################################################################

SET(sources
    main.cpp
    TCPServer.cpp
    Device.cpp
    DeviceManager.cpp

    Parser/Parser.cpp
    Parser/MemoryParser.cpp
    Parser/DiskParser.cpp
    Parser/BlobParser.cpp
    Frame.cpp
    Pair.cpp
    Grid.cpp
    PointCloud.cpp

    utilities.cpp

    Mobot/Mobot.cpp

    # cuSIFT
    lib/cuSIFT/cudaImage.cu
    lib/cuSIFT/cudaSiftH.cu
    lib/cuSIFT/extras/debug.h
    lib/cuSIFT/extras/matching.cu
    lib/cuSIFT/extras/rigidTransform.cu

    # readerwriterqueue
    lib/readerwriterqueue/atomicops.h
    lib/readerwriterqueue/readerwriterqueue.h

    # json
    lib/json/src/json.hpp
)

# Create the executable
CUDA_ADD_EXECUTABLE(MobotServer ${sources})

# Add linked libraries
TARGET_LINK_LIBRARIES(MobotServer ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(MobotServer ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(MobotServer ${OSMESA_LIBRARY})
TARGET_LINK_LIBRARIES(MobotServer ${CERES_LIBRARIES})

# Add extra flags
SET_TARGET_PROPERTIES(MobotServer PROPERTIES COMPILE_FLAGS "${EXTRA_CXX_FLAGS}")
# TARGET_COMPILE_DEFINITIONS(Rewrite PUBLIC REWRITE)