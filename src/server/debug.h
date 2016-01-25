#ifndef MAIN_DEBUG_H
#define MAIN_DEBUG_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <Eigen/Dense>
#include "ceres/rotation.h"
#include "glog/logging.h"

#include "json/src/json.hpp"

#include "TCPServer.h"
#include "DeviceManager.h"
#include "Mobot/Mobot.h"
#include "Grid.h"
#include "Pair.h"
#include "Frame.h"

#include "cuSIFT/extras/matching.h"
#include "cuSIFT/extras/rigidTransform.h"
#include "cuSIFT/extras/debug.h"

#include "Cerberus/Cerberus.h"
#include "Cerberus/Residuals.h"

 #include "cuBoF/cuBoF.h"
 #include "cuBoF/utils.h"

const int NUM_DEVICES = 1;

using json = nlohmann::json;
using namespace std;

void buildGrid();
void readDataFromBlobToDisk();
void readDataFromTCPToMemory();
void readDataFromBlobToMemory();
void collectWheelToHeadCalibrationDataFromRobot();
void testOccupancyGridCalculation();
void testSift();
void testMatching();
void testMatchingNearlySameImage();
void testCerberus();
void testCUBOFCreate();
void testCUBOF();
void testRandomIntVector();
void testCeresRotationMatrix();
void testReadArrayFromMatlab();
void testReadIndicesFromMatlab();
int testBundleAdjustment(int argc, char *argv[]);
void testReadFloatSift();
void compareIndices();

#endif