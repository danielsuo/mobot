#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <vector>
#include <string>

#include <unistd.h>     // usleep
#include <arpa/inet.h>  // Network address manipulation functions

#include "Device.h"
#include "Parser/Parser.h"
#include "Pair.h"
#include "Frame.h"

#define MAX_NUM_FAILED_POLLS 50
#define MAX_USEC_FRAME_WINDOW 50

class DeviceManager {
public:
  vector<Device *> devices;
  vector<Frame *> frames;

  int numFailedPolls;

  DeviceManager();
  ~DeviceManager();

  void runLoop();
  void pollDevices(Frame *frame);
  void addDeviceByFilePath(char *name, char *path, ParserOutputMode mode);
  void addDeviceByStringIPAddress(char *name, char *address, int port, ParserOutputMode mode);
  void addDeviceByIPAddress(uint32_t addr, uint16_t port, ParserOutputMode mode);

  Device *getDeviceByIPAddress(uint32_t addr, uint16_t port);

  void digest();
};

#endif
