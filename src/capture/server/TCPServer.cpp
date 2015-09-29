#include "TCPServer.h"
#include "handler_client.h"

TCPServer::TCPServer(int port) {
  this->port = port;
}

void TCPServer::listen() {
  printf("Listening on %d\n", this->port);
  int rc = pthread_create(&this->listener, NULL, handler_client, this);
  if (rc) {
    printf("ERROR: return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
}

void *handler_device(void *device_pointer) {
  Device *device = (Device *)device_pointer;

  device->connect();
  device->ping();
  // device->setFileModeTCP();
  // device->startRecording();
  // device->updateTimeDiff();

  return 0;
}

void TCPServer::connect() {
  for(std::vector<Device *>::iterator iter = this->devices.begin(); iter != this->devices.end(); ++iter) {
    pthread_t cmd_thread;
    int rc = pthread_create(&cmd_thread, NULL, handler_device, *iter);
    if (rc) {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}

void TCPServer::add_device(Device *device) {
  if (std::find(this->devices.begin(), this->devices.end(), device) == this->devices.end()) {
    this->devices.push_back(device);
  }
}

// void TCPServer::add_device_by_addr(uint32_t addr) {
//   if (this->check_device_addr(addr)) {
//     Device *device = new Device((char *)"Test", addr, port);
//     this->devices.push_back(device);
//   }
// }

void TCPServer::remove_device(Device *device) {
  this->devices.erase(remove(this->devices.begin(), this->devices.end(), device), this->devices.end());
}

Device *TCPServer::get_device(uint32_t addr, uint16_t port) {
  for(std::vector<Device *>::iterator iter = this->devices.begin(); iter != this->devices.end(); ++iter) {
    if (addr == (*iter)->addr && port == (*iter)->port) {
      return *iter;
    }
  }

  return new Device(addr, port);
}

// bool TCPServer::check_device_addr(uint32_t addr) {
//   for(std::vector<Device *>::iterator iter = this->devices.begin(); iter != this->devices.end(); ++iter) {
//     struct in_addr tmp_addr;
//     tmp_addr.s_addr = (*iter)->host;
//     printf("%s \n", inet_ntoa(tmp_addr));
//     if (addr == (*iter)->host) {
//       return false;
//     }
//   }

//   return true;
// }