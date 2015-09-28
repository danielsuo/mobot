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
  struct timeval tp1;
  struct timeval tp2;
  // struct timeval tp3;

  double remote_timestamp;
  printf("Socket %d:\n", this->cmd_fd);
  int n;

  while (1) {
    sleep(1);
    gettimeofday(&tp1, NULL);
    double ms1 = tp1.tv_sec * 1000 + tp1.tv_usec / 1000.0;

    n = write(this->cmd_fd, &ms1, sizeof(unsigned long));
    if (n < 0) {
      perror("ERROR writing to socket");
      continue;
    }

    n = read(this->cmd_fd, &remote_timestamp, 255);
    if (n < 0) {
      perror("ERROR reading from socket");
      continue;
    }
    gettimeofday(&tp2, NULL);

    printf("%llu\n", remote_timestamp);

    double ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000.0;
    // double ms3 = tp3.tv_sec * 1000 + tp3.tv_usec / 1000;
    double ms = ms2 - ms1;
    double time_diff = remote_timestamp - (ms1 + ms / 2);

    this->_time_diff->add(time_diff);

    printf("Device: %s\n", this->name);
    printf("---------------------------\n");

    printf("%f time diff (device - server)\n", time_diff);
    printf("%f ms elapsed\n", ms);
    // printf("ms1: %ld\nms2: %ld\nms3: %ld\n", ms1, ms2, ms3);
    printf("ms1: %f\nms2: %f\n", ms1, ms2);

    printf("%f avg diff\n", this->getTimeDiff());
    printf("\n");
  }
}

double Device::getTimeDiff() {
  return this->_time_diff->avg();
}

void Device::disconnect() {
  close(this->cmd_fd);
}