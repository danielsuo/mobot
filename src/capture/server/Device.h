#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>

// Network address manipulation functions
#include <arpa/inet.h>

#include "MovingAverage.h"

#ifndef DEVICE_H
#define DEVICE_H

#define AVERAGE_TIME_DIFF_OVER_NUM_PINGS   100

class Device {
  public:
    char *          name;

    // Host address in network byte order (big endian)
    uint32_t        host;
    unsigned int    port;
    unsigned int    num_frames_received;
    int             cmd_fd;
    int             dat_fd;

    void connect();
    void ping();
    void disconnect();
    double getTimeDiff();
    Device();
    Device(char *name, uint32_t host, unsigned int port);
    Device(char *name, char *host, unsigned int port);
    ~Device();

  private:
    MovingAverage *_time_diff;
    void init(char *name, uint32_t host, unsigned int port);
};

#endif