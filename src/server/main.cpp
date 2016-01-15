/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>

#include "lib/json/src/json.hpp"

#include "TCPServer.h"
#include "DeviceManager.h"
#include "Mobot/Mobot.h"
#include "Grid.h"
#include "Pair.h"
#include "Frame.h"

#include "matching.h"
#include "rigidTransform.h"

using json = nlohmann::json;

const int NUM_DEVICES = 1;

void buildGrid();
void readDataFromBlobToDisk();
void readDataFromTCPToMemory();
void readDataFromBlobToMemory();
void collectWheelToHeadCalibrationDataFromRobot();
void testOccupancyGridCalculation();
void testSift();
void testMatching();
void testMatchingNearlySameImage();

int main(int argc, char *argv[]) {

  // testSift();
  readDataFromBlobToMemory();
  return 0;
}

void testSift() {
  string device1_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg";
  string device1_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png";
  Pair *pair = new Pair(device1_color, device1_depth);

  // for (int i = 0; i < pair->siftData.numPts; i++) {
  //   for (int j = 0; j < 128; j++) {
  //     fprintf(stderr, "%0.1f ", pair->siftData.h_data[i].data[j]);
  //   }
  //   fprintf(stderr,"\n");
  // }
}

void testMatching() {
  string pair1_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg";
  string pair1_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png";
  string pair2_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.50.113-0000000205.jpg";
  string pair2_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.50.113-0000000205.png";

   Pair *device1 = new Pair(pair1_color, pair1_depth);
  Pair *device2 = new Pair(pair2_color, pair2_depth);

  cv::Mat curr_match(0, 3, cv::DataType<float>::type);
  cv::Mat next_match(0, 3, cv::DataType<float>::type);

  int numMatchedPoints = device1->getMatched3DPoints(device2, curr_match, next_match);
  fprintf(stderr, "Number filtered matched points: %d\n", numMatchedPoints);
}

void testMatchingNearlySameImage() {
  string pair1_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg";
  string pair1_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png";
  string pair2_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.381-0000000164.jpg";
  string pair2_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.381-0000000164.png";

  Pair *pair11 = new Pair(pair1_color, pair1_depth);
  Pair *pair12 = new Pair(pair2_color, pair2_depth);
  Pair *pair21 = new Pair(pair1_color, pair1_depth);
  Pair *pair22 = new Pair(pair2_color, pair2_depth);

  MatchSiftData(pair11->siftData, pair12->siftData, MatchSiftDistanceL2);
  MatchSiftData(pair21->siftData, pair22->siftData, MatchSiftDistanceDotProduct);

  SiftPoint *siftPoints1 = pair11->siftData.h_data;
  SiftPoint *siftPoints2 = pair21->siftData.h_data;

  for (int i = 0; i < pair11->siftData.numPts; i++) {
    
    fprintf(stderr, "Matches: L2 (%0.4f, %0.4f) <> (%0.4f, %0.4f), DP (%0.4f, %0.4f) <> (%0.4f, %0.4f)\n", 
      siftPoints1[i].xpos, siftPoints1[i].ypos, siftPoints1[i].match_xpos, siftPoints1[i].match_ypos,
      siftPoints2[i].xpos, siftPoints2[i].ypos, siftPoints2[i].match_xpos, siftPoints2[i].match_ypos);
  }

  Pair *device1 = new Pair(pair1_color, pair1_depth);
  Pair *device2 = new Pair(pair2_color, pair2_depth);

  cv::Mat curr_match(0, 3, cv::DataType<float>::type);
  cv::Mat next_match(0, 3, cv::DataType<float>::type);

  int numMatchedPoints = device1->getMatched3DPoints(device2, curr_match, next_match);
  fprintf(stderr, "Number filtered matched points: %d\n", numMatchedPoints);
}

void buildGrid() {
  Grid *grid = new Grid(11, 11);

  for (int i = 0; i < grid->height; i++) {
    for (int j = 0; j < grid->width; j++) {
      if ((i + j) % 2 == 0) grid->set(i, j, true);
    }
  }

  grid->print();
  grid->resize(GridSideLeft, 1);
  grid->print();
  grid->resize(GridSideUp, 1);
  grid->print();
  grid->resize(GridSideDown, 1);
  grid->print();
  grid->resize(GridSideRight, 1);
  grid->print();

  grid->shift(1, 1);
  grid->print();
  grid->shift(-1, 1);
  grid->print();
  grid->shift(5, 0);
  grid->print();
}

void readDataFromBlobToDisk() {
  DeviceManager *manager = new DeviceManager();
  manager->addDeviceByFilePath((char *)"device1", (char *)"device1", ParserOutputModeDisk);
  manager->addDeviceByFilePath((char *)"device2", (char *)"device2", ParserOutputModeDisk);
  manager->addDeviceByFilePath((char *)"device3", (char *)"device3", ParserOutputModeDisk);
  manager->addDeviceByFilePath((char *)"device4", (char *)"device4", ParserOutputModeDisk);

  manager->digest();
}

void readDataFromTCPToMemory() {
  TCPServer *server = new TCPServer(8125);
  DeviceManager *manager = server->manager;

  // manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124);
  manager->addDeviceByStringIPAddress((char *)"iPhone", (char *)"192.168.0.109", 8124, ParserOutputModeMemory);

  server->connect();
  server->listen();
  manager->runLoop();

  pthread_exit(NULL);
}

void readDataFromBlobToMemory() {
  json config;

  ifstream configFile;
  configFile.open("../config/main.json");
  configFile >> config;
  configFile.close();

  cout << config.dump(4) << endl;

  DeviceManager *manager = new DeviceManager();

  for (int i = 0; i < NUM_DEVICES; i++) {

    string name = config["devices"][i]["name"];
    string path = config["devices"][i]["path"];
    manager->addDeviceByFilePath((char *)name.c_str(), (char *)path.c_str(), ParserOutputModeMemory);

    vector<float> extrinsic = config["devices"][i]["extrinsicMatrixRelativeToFirstCamera"];
    manager->devices[i]->extrinsicMatrixRelativeToFirstCamera = new float[12];

    for (int j = 0; j < 12; j++) {
      manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[j] = extrinsic[j];
    }

    manager->devices[i]->scaleRelativeToFirstCamera = config["devices"][i]["scaleRelativeToFirstCamera"];
  }

  manager->digest();

  for (int i = 0; i < NUM_DEVICES; i++) {
    string timestamp_path = config["devices"][i]["timestamp_path"];
    manager->devices[i]->readTimestamps((char *)timestamp_path.c_str());
  }

  manager->runLoop();

  delete manager;
}

void collectWheelToHeadCalibrationDataFromRobot() {
  TCPServer *server = new TCPServer(8124);
  DeviceManager *manager = server->manager;

  manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124, ParserOutputModeBlob);
  manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124, ParserOutputModeBlob);
  manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124, ParserOutputModeBlob);
  manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124, ParserOutputModeBlob);

  server->connect();
  server->listen();

  Mobot mobot((char *)"192.168.0.129", 8125);
  mobot.connect();
  mobot.listen();

  sleep(5);

  mobot.turn(-360);

  while (mobot.progress < 100) {
    fprintf(stderr, "Current progress: %d\n", mobot.progress);
    sleep(1);
  }
  
  server->disconnect();
  mobot.disconnect();
}

void testOccupancyGridCalculation() {
  string device1_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg";
  string device1_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png";
  string device2_color = "image_data/device2/2016-01-11T10.11.28.502/color/2016-01-11T10.11.31.475-0000000177.jpg";
  string device2_depth = "image_data/device2/2016-01-11T10.11.28.502/depth/2016-01-11T10.11.31.475-0000000177.png";
  string device3_color = "image_data/device3/2016-01-11T10.10.44.791/color/2016-01-11T10.10.47.760-0000000191.jpg";
  string device3_depth = "image_data/device3/2016-01-11T10.10.44.791/depth/2016-01-11T10.10.47.760-0000000191.png";
  string device4_color = "image_data/device4/2016-01-11T10.10.44.358/color/2016-01-11T10.10.47.326-0000000133.jpg";
  string device4_depth = "image_data/device4/2016-01-11T10.10.44.358/depth/2016-01-11T10.10.47.326-0000000133.png";

  Pair *device1 = new Pair(device1_color, device1_depth);
  Pair *device2 = new Pair(device2_color, device2_depth);
  Pair *device3 = new Pair(device3_color, device3_depth);
  Pair *device4 = new Pair(device4_color, device4_depth);

  Frame *frame = new Frame(4);
  frame->pairs[0] = device1;
  frame->pairs[1] = device2;
  frame->pairs[2] = device3;
  frame->pairs[3] = device4;

  fprintf(stderr, "Num pairs %lu\n", frame->pairs.size());

  json config;

  ifstream configFile;
  configFile.open("../config/main.json");
  configFile >> config;
  configFile.close();

  cout << config.dump(4) << endl;

  for (int i = 0; i < 4; i++) {
    float tmp[12];
    for (int j = 0; j < 12; j++) {
      tmp[j] = config["devices"][i]["extrinsicMatrixRelativeToFirstCamera"][j];
      fprintf(stderr, "%0.4f ", tmp[j]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "Point cloud size: %d\n", frame->pairs[i]->pointCloud->color.size().height *  frame->pairs[i]->pointCloud->color.size().width);
    frame->buildPointCloud(i, 1, tmp);
  }

  frame->pointCloud_world->copy(frame->pointCloud_camera);
  frame->writePointCloud();

  float minx, maxx, miny, maxy, minz, maxz;
  frame->pointCloud_world->getExtents(minx, maxx, miny, maxy, minz, maxz);

  fprintf(stderr, "Bounds x: (%0.4f, %0.4f), y: (%0.4f, %0.4f), z: (%0.4f, %0.4f)\n", minx, maxx, miny, maxy, minz, maxz);
}