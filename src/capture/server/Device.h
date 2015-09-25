#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>

#include "MovingAverage.h"

#ifndef DEVICE_H
#define DEVICE_H

#define AVERAGE_TIME_DIFF_OVER_NUM_PINGS   100

class Device {
  char *          name;
  char *          host;
  unsigned int    port;
  unsigned int    num_frames_received;
  int             cmd_fd;
  int             dat_fd;

  public:
    void connect();
    void ping();
    void disconnect();
    double getTimeDiff();
    Device();
    Device(char *name, char *host, unsigned int port);
    ~Device();

  private:
    MovingAverage *_time_diff;
};

#endif