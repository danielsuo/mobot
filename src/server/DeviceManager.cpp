#include "DeviceManager.h"

DeviceManager::DeviceManager() {
  
}

DeviceManager::~DeviceManager() {
  for (int i = 0; i < numDevices; i++) {
    delete devices[i];
  }

  for (int i = 0; i < frames.size(); i++) {
    delete frames[i];
  }
}

void DeviceManager::addDeviceByFileDescriptor(char *name, int fd) {
  Device *device = new Device(name, fd);

  // Likely we are processing from disk blob to disk files
  device->parser->preprocessor = disk_preprocessor;
  device->parser->processor = disk_processor;
  device->parser->writer = disk_writer;

  devices.push_back(device);
  numDevices++;
}

void DeviceManager::addDeviceByIPAddress(char *name, char *address, int port) {
  Device *device = new Device(name, address, port);
  devices.push_back(device);
  numDevices++;
}

void DeviceManager::digest() {
  for (int i = 0; i < numDevices; i++) {
    devices[i]->digest();
  }
}