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
  // vector<pthread_t> cmd_threads;
  // vector<pthread_t> dat_threads;

  public:
    int port;
    int accept_socket;
    double start_time;
    vector<Device *> devices;

    TCPServer(int port);
    void listen();
    void connect();
    void add_device(Device *device);
    void remove_device(Device *device);
    Device *get_device(uint32_t addr, uint16_t port);
    // void add_device_by_addr(uint32_t addr);
    // bool check_device_addr(uint32_t addr);
};

#endif