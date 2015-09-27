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
    uint32_t        addr;
    uint16_t        port;
    unsigned int    num_frames_received;
    int             cmd_fd;
    int             dat_fd;

    // Constructors & Destructors
    Device();
    Device(uint32_t addr, uint16_t port);
    Device(char *name, char *addr, uint16_t port);
    ~Device();

    // Operators
    friend bool operator== (Device &device1, Device &device2);

    // Methods
    void connect();
    void ping();
    void disconnect();
    double getTimeDiff();

  private:
    MovingAverage *_time_diff;
    void init(uint32_t addr, uint16_t port);
};

#endif