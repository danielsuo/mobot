#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <vector>
#include <string>
#include <arpa/inet.h>  // Network address manipulation functions

#include "Device.h"
#include "Frame.h"

#include "data_processors.h"
#include "data_writers.h"

typedef enum {
  DeviceOutputModeBlob,
  DeviceOutputModeDisk,
  DeviceOutputModeMemory
} DeviceOutputMode;

class DeviceManager {
public:
  vector<Device *> devices;
  vector<Frame *> frames;

  int numDevices;
  DeviceOutputMode mode;

  DeviceManager(DeviceOutputMode mode);
  ~DeviceManager();

  void addDeviceByFileDescriptor(char *name, int fd);
  void addDeviceByStringIPAddress(char *name, char *address, int port);
  void addDeviceByIPAddress(uint32_t addr, uint16_t port);

  Device *getDeviceByIPAddress(uint32_t addr, uint16_t port);

  void digest();

private:
  void initDevice(Device *device);
};

#endif