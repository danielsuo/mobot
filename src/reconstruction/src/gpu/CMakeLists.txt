cmake_minimum_required(VERSION 2.6)

project(alignTwoRGBDGPU)
set(CPACK_GENERATOR "ZIP")
include(CPack)

SET("OpenCV_DIR" "/usr/local/Cellar/opencv3/3.0.0/share/OpenCV")

# Try to find Mesa off-screen library and include dir.
# Once done this will define
#
# OSMESA_FOUND        - true if OSMesa has been found
# OSMESA_INCLUDE_DIR  - where the GL/osmesa.h can be found
# OSMESA_LIBRARY      - Link this to use OSMesa


if(NOT OSMESA_INCLUDE_DIR)

  # If we have a root defined look there first
  if(OSMESA_ROOT)
    find_path(OSMESA_INCLUDE_DIR GL/osmesa.h PATHS ${OSMESA_ROOT}/include
      NO_DEFAULT_PATH
    )
  endif()

  if(NOT OSMESA_INCLUDE_DIR)
    find_path(OSMESA_INCLUDE_DIR GL/osmesa.h PATHS
      /usr/openwin/share/include
      /opt/graphics/OpenGL/include
    )
  endif()
endif()

# This may be left blank if OSMesa symbols are included
# in the main Mesa library
if(NOT OSMESA_LIBRARY)
  # If we have a root defined look there first
  if(OSMESA_ROOT)
    find_library(OSMESA_LIBRARY OSMesa PATHS ${OSMESA_ROOT}/lib
      NO_DEFAULT_PATH
    )
  endif()

  if(NOT OSMESA_LIBRARY)
    find_library(OSMESA_LIBRARY OSMesa PATHS
      /opt/graphics/OpenGL/lib
      /usr/openwin/lib
    )
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OSMesa  DEFAULT_MSG  OSMESA_LIBRARY  OSMESA_INCLUDE_DIR)

mark_as_advanced(OSMESA_INCLUDE_DIR OSMESA_LIBRARY)

find_package(OpenCV REQUIRED)
find_package(CUDA)
if (NOT CUDA_FOUND)
  message(STATUS "CUDA not found. Project will not be built.")
endif(NOT CUDA_FOUND)

if (WIN32)
  set(EXTRA_CXX_FLAGS "/DVERBOSE /D_CRT_SECURE_NO_WARNINGS ")
  list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;-O2;-DVERBOSE") 
endif()
if (UNIX)
  if (APPLE)
    set(EXTRA_CXX_FLAGS "-DVERBOSE -msse2 ")
    list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;-O2;-DVERBOSE") 
  else()
    set(EXTRA_CXX_FLAGS "-DVERBOSE -msse2 -std=c++0x ")
    list(APPEND CUDA_NVCC_FLAGS "-arch=sm_20;--compiler-options;-O2;-DVERBOSE") 
  endif()
endif()

set(cuda_sources
  cudaImage.cu  
  cudaImage.h  
  cudaSiftH.cu 
  cudaSiftH.h  
  matching.cu  
  cudaSiftD.h  
  cudaSift.h  
  cudautils.h
  estimatRTGPU.cu # gpu ransac
)  

set(sources
  #estimatRT.cpp # cpu ransac
  #geomFuncs.cpp  
  mainSift.cpp
  RGBD_utils.cpp
  Frame.cpp
  Pair.cpp
  Parameters.cpp
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR} ${OSMESA_INCLUDE_DIR}
)

cuda_add_executable(alignTwoRGBDGPU ${cuda_sources} ${sources})


set_target_properties(alignTwoRGBDGPU PROPERTIES
  COMPILE_FLAGS "${EXTRA_CXX_FLAGS}"			   
)

target_link_libraries(alignTwoRGBDGPU
  "${OpenCV_LIBS}" "${OSMESA_LIBRARY}"
)

install(FILES 
  ${cuda_sources} 
  ${sources}
  cudaSiftD.cu
  CMakeLists.txt
  Copyright.txt
  DESTINATION .
)