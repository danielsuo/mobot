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
#include "Mobot/Mobot.h"

using json = nlohmann::json;

const int NUM_DEVICES = 4;

int main(int argc, char *argv[]) {

  // TCPServer *server = new TCPServer(8124);
  // DeviceManager *manager = server->manager;

  // manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124, ParserOutputModeBlob);
  // manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124, ParserOutputModeBlob);
  // manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124, ParserOutputModeBlob);
  // manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124, ParserOutputModeBlob);

  // server->connect();
  // server->listen();

  // Mobot mobot((char *)"192.168.0.129", 8125);
  // mobot.connect();
  // mobot.listen();

  // sleep(5);

  // mobot.turn(-360);

  // while (mobot.progress < 100) {
  //   fprintf(stderr, "Current progress: %d\n", mobot.progress);
  //   sleep(1);
  // }
  
  // server->disconnect();
  // mobot.disconnect();

  DeviceManager *manager = new DeviceManager();
  manager->addDeviceByFilePath((char *)"device1", (char *)"device1", ParserOutputModeDisk);
  manager->addDeviceByFilePath((char *)"device2", (char *)"device2", ParserOutputModeDisk);
  manager->addDeviceByFilePath((char *)"device3", (char *)"device3", ParserOutputModeDisk);
  manager->addDeviceByFilePath((char *)"device4", (char *)"device4", ParserOutputModeDisk);

  manager->digest();

/*
  bool file = true;

  json config;

  ifstream configFile;
  configFile.open("../config/main.json");
  configFile >> config;
  configFile.close();

  cout << config.dump(4) << endl;

  if (file) {
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
  */

  pthread_exit(NULL);
  return 0;
}