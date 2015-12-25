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
    DeviceManager *manager = new DeviceManager(DeviceOutputModeMemory);

    for (int i = 0; i < NUM_DEVICES; i++) {
      string name = config["devices"][i]["name"];
      string path = config["devices"][i]["path"];
      manager->addDeviceByFilePath((char *)name.c_str(), (char *)path.c_str());

      vector<float> extrinsic = config["devices"][i]["extrinsicMatrixRelativeToFirstCamera"];
      manager->devices[i]->extrinsicMatrixRelativeToFirstCamera = new float[12];

      for (int j = 0; j < 12; j++) {
        manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[j] = extrinsic[j];
      }

      manager->devices[i]->scaleRelativeToFirstCamera = config["devices"][i]["scaleRelativeToFirstCamera"];
    }

    manager->digest();
    manager->runLoop();

    delete manager;
  } else {
    TCPServer *server = new TCPServer(8125, DeviceOutputModeMemory);
    DeviceManager *manager = server->manager;

  // manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124);
    manager->addDeviceByStringIPAddress((char *)"iPhone", (char *)"192.168.0.109", 8124);

    server->connect();
    server->listen();
    manager->runLoop();
  }

  pthread_exit(NULL);
  return 0;
}


// [1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0],
// [0.0018899, 0.37744, -0.92603, -0.67797, 0.6812, 0.27627, 0.73509, 0.6273, 0.25718, 0.19325, -0.0093373, -0.024299],
// [-0.99963, -0.024081, -0.012792, -0.024453, 0.58407, 0.81133, -0.012066, 0.81134, -0.58445, 0.17644, 0.031571, -0.19235],
// [0.023999, 0.11389, 0.9932, 0.6482, 0.75458, -0.10219, -0.76109, 0.64624, -0.055715, -0.046145, -0.01405, -0.16973],
