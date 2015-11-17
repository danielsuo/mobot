#include "TCPServer.h"
#include "handler_client.h"

TCPServer::TCPServer(int port) {
  this->port = port;

  struct timeval t;
  gettimeofday(&t, NULL);
  this->start_time = t.tv_sec * 1000 + t.tv_usec / 1000.0;
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
  for (std::vector<Device *>::iterator iter = this->devices.begin(); iter != this->devices.end(); ++iter) {
    int rc = pthread_create(&(*iter)->cmd_thread, NULL, handler_device, *iter);
    if (rc) {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}

void TCPServer::add_device(Device *device) {
  if (std::find(this->devices.begin(), this->devices.end(), device) == this->devices.end()) {
    this->devices.push_back(device);
    device->server = this;
  }
}

void TCPServer::remove_device(Device *device) {
  this->devices.erase(remove(this->devices.begin(), this->devices.end(), device), this->devices.end());
}

Device *TCPServer::get_device(uint32_t addr, uint16_t port) {

  struct sockaddr_in saddr;

  saddr.sin_addr.s_addr = addr;
  printf("Looking for device with address %s:%u\n", inet_ntoa(saddr.sin_addr), port);
  for (std::vector<Device *>::iterator iter = this->devices.begin(); iter != this->devices.end(); ++iter) {

    saddr.sin_addr.s_addr = (*iter)->addr;
    printf("Found device with address %s:%u\n", inet_ntoa(saddr.sin_addr), (*iter)->dat_port);

    // Authenticate with address only, not port
    if (addr == (*iter)->addr) {
      printf("Found existing device!\n");

      // Update cmd_port
      (*iter)->dat_port = port;
      return *iter;
    }
  }

  printf("Creating new device!\n");

  Device *device = new Device(addr, port);
  this->add_device(device);

  return device;
}

// void TCPServer::add_device_by_addr(uint32_t addr) {
//   if (this->check_device_addr(addr)) {
//     Device *device = new Device((char *)"Test", addr, port);
//     this->devices.push_back(device);
//   }
// }

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