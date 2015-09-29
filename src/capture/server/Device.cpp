#include "Device.h"

void Device::init(uint32_t addr, uint16_t port) {
  this->port = port;
  this->addr = addr;

  this->num_frames_received = 0;

  this->_time_diff = new MovingAverage(AVERAGE_TIME_DIFF_OVER_NUM_PINGS);
}

Device::Device(uint32_t addr, uint16_t port) {
  init(addr, port);
}

Device::Device(char *name, char *addr, uint16_t port) {
  this->name = name;
  init(inet_addr(addr), port);
}

Device::~Device() {
  close(this->cmd_fd);
}

void Device::connect() {
  struct sockaddr_in serv_addr;

  this->cmd_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (this->cmd_fd < 0) {
    perror("ERROR: opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = this->addr;
  printf("Connecting to %s:%i\n", inet_ntoa(serv_addr.sin_addr), this->port);

  serv_addr.sin_port = htons(this->port);
  if (::connect(this->cmd_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
  }
}

bool operator== (Device &device1, Device &device2) {
  return device1.addr == device2.addr;
}

void Device::ping() {
  while(1) {
    this->updateTimeDiff();
  }
}

void Device::disconnect() {
  close(this->cmd_fd);
}

double Device::getTimeDiff() {
  return this->_time_diff->avg();
}

void Device::sendCommand(uint8_t cmd, const void *args, uint8_t arglen) {
  write(this->cmd_fd, &cmd, sizeof(uint8_t));
  if (args) {
    write(this->cmd_fd, &arglen, sizeof(uint8_t));
    write(this->cmd_fd, args, arglen);
  }
}

void Device::updateTimeDiff() {
  printf("Updating time diff...\n");

  struct timeval tp1;
  struct timeval tp2;
  double mach_time;

  sleep(1);
  gettimeofday(&tp1, NULL);
  double ms1 = tp1.tv_sec * 1000 + tp1.tv_usec / 1000.0;

  this->sendCommand(TCPDeviceCommandGetMachTime, NULL, 0);

  int n = read(this->cmd_fd, &mach_time, 255);

  if (n < 0) {
    perror("ERROR reading from socket");
    return;
  }

  printf("Device: %s\n", this->name);
  printf("---------------------------\n");

  printf("mach_time: %f\n", mach_time);
  gettimeofday(&tp2, NULL);
  double ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000.0;

  double ms = ms2 - ms1;
  double time_diff = mach_time - (ms1 + ms / 2);

  this->_time_diff->add(time_diff);
  printf("%f time diff (device - server)\n", time_diff);
  printf("%f ms elapsed\n", ms);
  printf("ms1: %f\nms2: %f\n", ms1, ms2);

  printf("%f avg diff\n", this->getTimeDiff());
  printf("\n");
}

void Device::startRecording() {
  printf("Start recording...\n");
  this->sendCommand(TCPDeviceCommandStartRecording, NULL, 0);
}

void Device::setFileModeTCP() {
  printf("Setting mode to tcp...\n");
  this->sendCommand(TCPDeviceCommandFileModeTCP, NULL, 0);
}