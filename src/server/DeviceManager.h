#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <vector>

#include "Device.h"
#include "Frame.h"

#include "data_processors.h"
#include "data_writers.h"

class DeviceManager {
  public:
    vector<Device *> devices;
    vector<Frame *> frames;

    int numDevices;

    DeviceManager();
    ~DeviceManager();

    void addDeviceByFileDescriptor(char *name, int fd);
    void addDeviceByIPAddress(char *name, char *address, int port);

    void digest();
};

#endif