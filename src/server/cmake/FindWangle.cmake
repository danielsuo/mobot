#  Copyright (c) 2014, Facebook, Inc.
#  All rights reserved.
#
#  This source code is licensed under the BSD-style license found in the
#  LICENSE file in the root directory of this source tree. An additional grant
#  of patent rights can be found in the PATENTS file in the same directory.
#
# - Try to find folly
# This will define
# WANGLE_FOUND
# WANGLE_INCLUDE_DIR
# WANGLE_LIBRARIES

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7 FATAL_ERROR)

INCLUDE(FindPackageHandleStandardArgs)

FIND_LIBRARY(WANGLE_LIBRARY wangle)
# FIND_PATH(WANGLE_INCLUDE_DIR "wangle/")

SET(WANGLE_LIBRARIES ${WANGLE_LIBRARY})
SET(WANGLE_INCLUDE_DIR "/usr/local/include/wangle")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Wangle
  REQUIRED_ARGS WANGLE_INCLUDE_DIR WANGLE_LIBRARIES)