#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <vector>
#include <string>

#include <unistd.h>     // usleep
#include <arpa/inet.h>  // Network address manipulation functions

#include "Device.h"
#include "Parser.h"
#include "Pair.h"
#include "Frame.h"

#define MAX_NUM_FAILED_POLLS 50

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

  int failedPolls;

  DeviceManager(DeviceOutputMode mode);
  ~DeviceManager();

  void runLoop();
  void addDeviceByFileDescriptor(char *name, int fd);
  void addDeviceByFilePath(char *name, char *path);
  void addDeviceByStringIPAddress(char *name, char *address, int port);
  void addDeviceByIPAddress(uint32_t addr, uint16_t port);

  Device *getDeviceByIPAddress(uint32_t addr, uint16_t port);

  void digest();

private:
  void initDevice(Device *device);
};

#endif
