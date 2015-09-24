#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>

 // #include <netinet/in.h>

#ifndef DEVICE_H
#define DEVICE_H

class Device {
  char *          name;
  char *          host;
  unsigned int    port;
  unsigned long   delay;
  unsigned int    num_frames_received;
  int             socket_fd;

  public:
    void connect();
    void ping();
    void disconnect();
    Device();
    Device(char *name, char *host, unsigned int port);
    ~Device();
};

#endif