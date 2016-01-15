#ifndef DEVICE_H
#define DEVICE_H

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

// Network address manipulation functions
#include <arpa/inet.h>

#include "Parser/Parser.h"
#include "utilities.h"

#define AVERAGE_TIME_DIFF_OVER_NUM_PINGS   100
#define PINGS_BEFORE_RECORD 2

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
  static int      currIndex;
  int             index;
  char *          name;
  char *          path;

  // Host address in network byte order (big endian)
  uint32_t        addr;
  uint16_t        cmd_port;
  uint16_t        dat_port;
  int             cmd_fd;
  int             dat_fd;
  float           *extrinsicMatrixRelativeToFirstCamera;
  float           scaleRelativeToFirstCamera;

  pthread_t       cmd_thread;
  pthread_t       dat_thread;

  Parser          *parser;

  // Constructors & Destructors
  Device();
  Device(char *name, char *path, ParserOutputMode mode);
  Device(uint32_t addr, uint16_t port, ParserOutputMode mode);
  Device(char *name, char *addr, uint16_t port, ParserOutputMode mode);
  ~Device();

  // Operators
  friend bool operator== (Device &device1, Device &device2);

  // Methods
  void digest();

  int connect();
  void ping(int times);
  void disconnect();
  double getTimeDiff();

  void readTimestamps(char *path);
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
  void init(char *name, uint32_t addr, uint16_t port, ParserOutputMode mode);
};

#endif