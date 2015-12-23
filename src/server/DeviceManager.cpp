#include "DeviceManager.h"

DeviceManager::DeviceManager(DeviceOutputMode mode) {
  // Parameters *parameters = new Parameters("../", "data/");
  this->mode = mode;
}

DeviceManager::~DeviceManager() {
  for (int i = 0; i < numDevices; i++) {
    delete devices[i];
  }

  for (int i = 0; i < frames.size(); i++) {
    delete frames[i];
  }
}

void DeviceManager::initDevice(Device *device) {
  device->manager = this;
  devices.push_back(device);
  numDevices++;

  switch (mode) {
    case DeviceOutputModeBlob:
    device->parser->preprocessor = blob_preprocessor;
    device->parser->processor = blob_processor;
    device->parser->writer = blob_writer;
    break;

    case DeviceOutputModeDisk:
    device->parser->preprocessor = disk_preprocessor;
    device->parser->processor = disk_processor;
    device->parser->writer = disk_writer;
    break;

    case DeviceOutputModeMemory:
    device->parser->preprocessor = memory_preprocessor;
    device->parser->processor = memory_processor;
    device->parser->writer = memory_writer;
    break;
  }
}

void DeviceManager::addDeviceByFileDescriptor(char *name, int fd) {
  Device *device = new Device(name, fd);

  // Likely we are processing from disk blob to disk files
  initDevice(device);
}

void DeviceManager::addDeviceByStringIPAddress(char *name, char *address, int port) {
  Device *device = new Device(name, address, port);

  initDevice(device);
}

void DeviceManager::addDeviceByIPAddress(uint32_t addr, uint16_t port) {
  Device *device = new Device(addr, port);

  initDevice(device);
}

Device *DeviceManager::getDeviceByIPAddress(uint32_t addr, uint16_t port) {
  struct sockaddr_in saddr;

  saddr.sin_addr.s_addr = addr;
  printf("Looking for device with address %s:%u\n", inet_ntoa(saddr.sin_addr), port);

  for (int i = 0; i < numDevices; i++) {
    saddr.sin_addr.s_addr = devices[i]->addr;
    printf("Found device with address %s:%u\n", inet_ntoa(saddr.sin_addr), devices[i]->dat_port);

    // Authenticate with address only, not port
    if (addr == devices[i]->addr) {
      printf("Found existing device!\n");

      // Update cmd_port
      devices[i]->dat_port = port;
      return devices[i];
    }
  }

  printf("Creating new device!\n");

  // We aren't expecting connections that aren't pre-defined
  addDeviceByIPAddress(addr, port);
  return devices[numDevices - 1];
}

void DeviceManager::digest() {
  for (int i = 0; i < numDevices; i++) {
    devices[i]->digest();
  }
}