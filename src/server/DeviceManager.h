#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <vector>

#include "Device.h"
#include "Frame.h"

class DeviceManager {
  public:
    vector<Device *> devices;
    vector<Frame *> frames;

    int numDevices;

    DeviceManager(int numDevices);
    ~DeviceManager();
};

#endif