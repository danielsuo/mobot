#include "TCPServer.h"
#include "handler_client.h"

TCPServer::TCPServer(int port, DeviceOutputMode mode) {
  this->port = port;
  manager = new DeviceManager(mode);

  struct timeval t;
  gettimeofday(&t, NULL);
  start_time = t.tv_sec * 1000 + t.tv_usec / 1000.0;
}

TCPServer::~TCPServer() {
  delete manager;
}

void TCPServer::listen() {
  printf("Listening on %d\n", port);
  int rc = pthread_create(&listener, NULL, handler_client, this);
  if (rc) {
    printf("ERROR: return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
}

void *handler_device(void *device_pointer) {
  Device *device = (Device *)device_pointer;

  if (device->connect() < 0) {
    fprintf(stderr, "Could not connect to %s\n", device->name);
    return 0;
  }

  device->setFileModeTCP();
  device->startRecording();
  device->ping(60);
  device->stopRecording();
  device->updateTimeDiff();

  return 0;
}

void TCPServer::connect() {
  for (int i = 0; i < manager->numDevices; i++) {
    Device *device = manager->devices[i];
    int rc = pthread_create(&device->cmd_thread, NULL, handler_device, device);
    if (rc) {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}
