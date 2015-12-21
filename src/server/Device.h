#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>

// Network address manipulation functions
#include <arpa/inet.h>

#include "MovingAverage.h"

#ifndef DEVICE_H
#define DEVICE_H

#define AVERAGE_TIME_DIFF_OVER_NUM_PINGS   100
#define PINGS_BEFORE_RECORD 5

class Data;

typedef enum {
  TCPDeviceCommandGetMachTime,
  TCPDeviceCommandStartRecording,
  TCPDeviceCommandStopRecording,
  TCPDeviceCommandUpload,
  TCPDeviceCommandFileModeTCP,
  TCPDeviceCommandDimScreen
} TCPDeviceCommand;

class Device {
public:
  char *          name;

  // Host address in network byte order (big endian)
  uint32_t        addr;
  uint16_t        cmd_port;
  uint16_t        dat_port;
  unsigned int    num_frames_received;
  int             cmd_fd;
  int             dat_fd;

  pthread_t       cmd_thread;
  pthread_t       dat_thread;

  Data            *data;

  // Constructors & Destructors
  Device();
  Device(uint32_t addr, uint16_t port);
  Device(char *name, char *addr, uint16_t port);
  ~Device();

  // Operators
  friend bool operator== (Device &device1, Device &device2);

  // Methods
  int connect();
  void ping(int times);
  void disconnect();
  double getTimeDiff();
  bool readyToRecord;

  void processTimestamp(char *path, double timestamp);

  // Commands
  void sendCommand(uint8_t cmd, const void *args, uint8_t arglen);
  void updateTimeDiff();
  void startRecording();
  void stopRecording();
  void upload();
  void setFileModeTCP();
  void dimScreen();

private:
  MovingAverage *_time_diff;
  void init(uint32_t addr, uint16_t port);
};

#endif