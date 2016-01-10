#include "Device.h"

int Device::currIndex = 0;

Device::Device(char *name, char *path, ParserOutputMode mode) {
  this->name = name;
  this->path = path;

  parser = ParserFactory::createParser(index, name, mode);
  parser->name = name;
  parser->readyToRecord = true;

  index = currIndex++;
  cerr << "File descriptor for device " << index << " is " << dat_fd << endl;
}

void Device::init(char *name, uint32_t addr, uint16_t port, ParserOutputMode mode) {
  this->name = name;
  this->addr = addr;
  this->path = nullptr;
  cmd_port = port;

  parser = ParserFactory::createParser(index, name, mode);
  parser->readyToRecord = false;

  _time_diff = new MovingAverage(AVERAGE_TIME_DIFF_OVER_NUM_PINGS);

  index = currIndex++;
}

Device::Device(uint32_t addr, uint16_t port, ParserOutputMode mode) {
  init((char *)"null_name", addr, port, mode);
}

Device::Device(char *name, char *addr, uint16_t port, ParserOutputMode mode) {
  init(name, inet_addr(addr), port, mode);
}

Device::~Device() {
  close(this->cmd_fd);
  close(this->dat_fd);
  delete(this->parser);
}

void Device::digest() {
  if (path != nullptr) {
    FILE *fp = std::fopen(path, "r+");
    dat_fd = fileno(fp);
  }

  parser->digest(dat_fd);
}

int Device::connect() {
  struct sockaddr_in serv_addr;

  cmd_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (cmd_fd < 0) {
    perror("ERROR: opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = addr;
  printf("Connecting to %s:%i\n", inet_ntoa(serv_addr.sin_addr), cmd_port);

  serv_addr.sin_port = htons(cmd_port);
  return ::connect(cmd_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
}

bool operator== (Device &device1, Device &device2) {
  return device1.addr == device2.addr;
}

void Device::ping(int times) {
  int counter = 0;
  while (counter < times) {
    this->updateTimeDiff();
    if (counter == PINGS_BEFORE_RECORD) {
      parser->readyToRecord = true;
    }
    counter++;
  }
}

void Device::disconnect() {
  close(cmd_fd);
}

double Device::getTimeDiff() {
  return _time_diff->avg();
}

void Device::readTimestamps(char *path) {
  int len = 1024;
  FILE *fp = fopen(path, "rb");

  char line[len];

  cerr << "Queue length: " << parser->queue_length << endl;

  for (int i = 0; i < parser->queue_length * 2; i++) {
    double timestamp = 0;
    fread((void *)&timestamp, sizeof(double), 1, fp);
    fgets(line, len, fp);

    if (i % 2 == 0) continue;

    Pair *pair;
    if(parser->queue->try_dequeue(pair)) {
      pair->timestamp = timestamp;
      if (parser->queue->try_enqueue(pair)) {
        fprintf(stderr, "Data read: %d %d %0.9f %s\n", i, parser->queue_length, pair->timestamp, line);
      } else {
        cerr << "Had an error enqueuing again!" << endl;
      }
    } else {
      cerr << "Had an error dequeing pair!" << endl;
    }

  }

  if (fgets(line, len, fp) != NULL) {
    cerr << "Derp: got extra lines " << line << endl;
  }

  fclose(fp);
}

void Device::processTimestamp(char *path, double timestamp) {
  printf("%s, %f, %f\n", path, timestamp, (timestamp + this->getTimeDiff()));
}

void Device::sendCommand(uint8_t cmd, const void *args, uint8_t arglen) {
  uint16_t dat = arglen << 8 | cmd;
  write(cmd_fd, &dat, sizeof(uint16_t));

  if (args) {
    write(cmd_fd, args, arglen);
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

  sendCommand(TCPDeviceCommandGetMachTime, NULL, 0);

  int n = read(cmd_fd, &mach_time, 255);

  if (n < 0) {
    perror("ERROR reading from socket");
    return;
  }

  gettimeofday(&tp2, NULL);
  double ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000.0;

  double ms = ms2 - ms1;
  double time_diff = (ms1 + ms / 2) - mach_time;

  _time_diff->add(time_diff);
  parser->time_diff = getTimeDiff();

  printf("Device: %s\n", name);
  printf("---------------------------\n");

  printf("mach_time: %f\n", mach_time);
  printf("%f time diff (server - device)\n", time_diff);
  printf("%f ms elapsed\n", ms);
  printf("ms1: %f\nms2: %f\n", ms1, ms2);

  printf("%f avg diff\n", getTimeDiff());
  printf("\n");
}

void Device::startRecording() {
  printf("Start recording...\n");
  sendCommand(TCPDeviceCommandStartRecording, NULL, 0);
}

void Device::stopRecording() {
  sendCommand(TCPDeviceCommandStopRecording, NULL, 0);
}
void Device::upload() {
  sendCommand(TCPDeviceCommandUpload, NULL, 0);
}

void Device::setFileModeTCP() {
  printf("Setting mode to tcp...\n");
  sendCommand(TCPDeviceCommandFileModeTCP, NULL, 0);
}

void Device::dimScreen() {
  sendCommand(TCPDeviceCommandDimScreen, NULL, 0);
}
