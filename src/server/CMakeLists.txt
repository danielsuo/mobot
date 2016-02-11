CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

PROJECT(MobotServer C CXX)

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
  set(EXTRA_CXX_FLAGS "/D_CRT_SECURE_NO_WARNINGS ")
  list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;") 
endif()
if (UNIX)
  if (APPLE)
    set(EXTRA_CXX_FLAGS "-msse2 -g")
    list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;") 
  else()
    set(EXTRA_CXX_FLAGS "-msse2 -std=c++0x -g")
    list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;") 
  endif()
endif()

# Add Ceres
FIND_PACKAGE(Ceres REQUIRED)

# Add Folly and Wangke
find_package (Folly REQUIRED)
find_package (Wangle REQUIRED)
find_package (Glog REQUIRED)
include_directories (${FOLLY_INCLUDE_DIR} ${WANGLE_INCLUDE_DIR} ${GLOG_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Add cuSIFT
# ADD_SUBDIRECTORY(lib/cuSIFT)

# Add Cerberus
ADD_SUBDIRECTORY(lib/Cerberus)

# Add json
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/json)

# Add cuBoF
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/lib/cuBoF)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/cuBoF/lib)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/cuBoF/lib/vlfeat)

if (UNIX)
  if (APPLE)
    LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/cuBoF/lib/vlfeat/bin/maci64)
  else()
    LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/cuBoF/lib/vlfeat/bin/glnxa64)
  endif()
endif()


###############################################################################
# Set sources
###############################################################################

SET(sources
    main.cpp
    TCPServer.cpp
    Device.cpp
    DeviceManager.cpp
    Frame.cpp
    Pair.cpp
    Grid.cpp
    PointCloud.cpp

    Parser/Parser.cpp
    Parser/MemoryParser.cpp
    Parser/DiskParser.cpp
    Parser/BlobParser.cpp

    Strategy/Strategy.cpp
    Strategy/BundleAdjustAllStrategy.cpp
    Strategy/ReplicateMatlabStrategy.cpp

    utilities.cpp

    Mobot/Mobot.cpp

    debug.cpp
)

# Create the executable
cuda_add_executable (MobotServer ${sources})

# Add linked libraries
target_link_libraries (MobotServer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (MobotServer ${OpenCV_LIBS})
target_link_libraries (MobotServer ${OSMESA_LIBRARY})
target_link_libraries (MobotServer ${CERES_LIBRARIES})
target_link_libraries (MobotServer Cerberus)
target_link_libraries (MobotServer cuSIFT)
target_link_libraries (MobotServer cuBoF)
target_link_libraries (MobotServer vl)
target_link_libraries (MobotServer ${FOLLY_LIBRARIES})
target_link_libraries (MobotServer ${WANGLE_LIBRARIES})
target_link_libraries (MobotServer ${GLOG_LIBRARIES})

set_property(TARGET MobotServer PROPERTY CXX_STANDARD 11)
set_property(TARGET MobotServer PROPERTY CXX_STANDARD_REQUIRED ON)


# Add extra flags
SET_TARGET_PROPERTIES(MobotServer PROPERTIES COMPILE_FLAGS "${EXTRA_CXX_FLAGS}")

# TODO: add back -O2