/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include <iostream>
#include <fstream>

#include "lib/json/src/json.hpp"

#include "TCPServer.h"
#include "DeviceManager.h"

using json = nlohmann::json;

#define NUM_DEVICES 4

int main(int argc, char *argv[]) {

  bool file = true;

  json config;

  ifstream configFile;
  configFile.open("../config/main.json");
  configFile >> config;
  configFile.close();

  cout << config.dump(4) << endl;

  if (file) {
    DeviceManager *manager = new DeviceManager();

    // manager->addDeviceByFilePath((char *)"device1", (char *)"device1");
    // manager->addDeviceByFilePath((char *)"device2", (char *)"device2");
    // manager->addDeviceByFilePath((char *)"device3", (char *)"device3");
    // manager->addDeviceByFilePath((char *)"device4", (char *)"device4");
    //
    // for (int i = 0; i < NUM_DEVICES; i++) {
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera = new float[12];
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[0] = 1;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[1] = 0;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[2] = 0;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[3] = 0;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[4] = 0;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[5] = 1;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[6] = 0;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[7] = 0;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[8] = 0;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[9] = 0;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[10] = 1;
    //   manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[11] = 0;
    //   manager->devices[i]->scaleRelativeToFirstCamera = 1.0;
    // }

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
  } else {
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
  }

  pthread_exit(NULL);
  return 0;
}