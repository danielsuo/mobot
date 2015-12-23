#include "DeviceManager.h"

DeviceManager::DeviceManager(int numDevices) {
  this->numDevices = numDevices;
}

DeviceManager::~DeviceManager() {
  for (int i = 0; i < numDevices; i++) {
    delete devices[i];
  }

  for (int i = 0; i < frames.size(); i++) {
    delete frames[i];
  }
}