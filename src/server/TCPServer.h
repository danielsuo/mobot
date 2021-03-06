#include <vector>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "DeviceManager.h"
#include "Device.h"

using namespace std;

#ifndef TCPSERVER_H
#define TCPSERVER_H

class TCPServer {
  pthread_t listener;

public:
  int port;
  int accept_socket;
  double start_time;
  DeviceManager *manager;

  TCPServer(int port);
  ~TCPServer();
  void listen();
  void connect();
  void disconnect();
};

#endif