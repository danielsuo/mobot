#include <vector>
#include <stdlib.h>
#include <pthread.h>

// Network address manipulation functions
#include <arpa/inet.h>

#include "Device.h"

using namespace std;

#ifndef TCPSERVER_H
#define TCPSERVER_H

class TCPServer {
  pthread_t listener;
  vector<Device *> devices;
  vector<pthread_t> cmd_threads;
  vector<pthread_t> dat_threads;

  public:
    int port;
    int accept_socket;

    TCPServer(int port);
    void start();
    void add_device(uint32_t addr);
    int check_device_addr(uint32_t addr);
  // void (*callback_connection)(int);
};

#endif