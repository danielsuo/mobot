#include "Device.h"
#include "Parser.h"
#include "DeviceManager.h"

int Device::currIndex = 0;

void Device::init(uint32_t addr, uint16_t port) {
  this->cmd_port = port;
  this->addr = addr;

  this->num_frames_received = 0;

  this->_time_diff = new MovingAverage(AVERAGE_TIME_DIFF_OVER_NUM_PINGS);
  this->parser = new Parser();
  this->parser->device = this;

  this->readyToRecord = false;

  index = currIndex++;
  queue = new ReaderWriterQueue<Pair *>(1000);

  queue_length = 0;
  path = NULL;
}

Device::Device(char *name, int dat_fd) {
  this->name = name;
  this->dat_fd = dat_fd;
  this->parser = new Parser();
  this->parser->device = this;

  this->readyToRecord = true;
  queue = new ReaderWriterQueue<Pair *>(1000);
  queue_length = 0;
  path = NULL;

  index = currIndex++;
}

Device::Device(char *name, char *path) {
  this->name = name;
  this->path = path;
  this->parser = new Parser();
  this->parser->device = this;

  this->readyToRecord = true;
  queue = new ReaderWriterQueue<Pair *>(1000);
  queue_length = 0;

  index = currIndex++;
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
  close(this->dat_fd);
  delete(this->parser);

  Pair *pair;
  while (queue->try_dequeue(pair)) {
    delete pair;
  }
  delete queue;
}

void *digester(void *ptr) {
  Device *device = (Device *)ptr;
  cerr << "Launching digest for device " << device->index << endl;
  device->parser->digest(device->dat_fd);
  pthread_exit(NULL);
}

void Device::digest() {
   cerr << "Path for device " << index << " " << path << endl;
  if (path != NULL) {
    FILE *fp = std::fopen(path, "r+");
    dat_fd = fileno(fp);
    std::vector<char> buf(2000); // char is trivally copyable
    std::fread(&buf[0], sizeof buf[0], buf.size(), fp);

    for(char n : buf) {
      std::cout << n;
    }

    return;
    cerr << "File descriptor for device " << index << " is " << dat_fd << endl;
  } else {
    cerr << "Why is path not null?" << endl;
  }

  parser->digest(dat_fd);
  // pthread_t digest_thread;
  // int rc = pthread_create(&digest_thread, NULL, digester, this);
  // if (rc) {
  //   printf("ERROR: return code from pthread_create() in Device::digest is %d\n", rc);
  //   exit(-1);
  // }
}

int Device::connect() {
  struct sockaddr_in serv_addr;

  this->cmd_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (this->cmd_fd < 0) {
    perror("ERROR: opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = this->addr;
  printf("Connecting to %s:%i\n", inet_ntoa(serv_addr.sin_addr), this->cmd_port);

  serv_addr.sin_port = htons(this->cmd_port);
  return ::connect(this->cmd_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
}

bool operator== (Device &device1, Device &device2) {
  return device1.addr == device2.addr;
}

void Device::ping(int times) {
  int counter = 0;
  while (counter < times) {
    this->updateTimeDiff();
    if (counter == PINGS_BEFORE_RECORD) {
      readyToRecord = true;
      fprintf(stderr, "READY TO RECORD!!!!!");
    }
    counter++;
  }
}

void Device::disconnect() {
  close(this->cmd_fd);
}

double Device::getTimeDiff() {
  return this->_time_diff->avg();
}

void Device::processTimestamp(char *path, double timestamp) {
  printf("%s, %f, %f\n", path, timestamp, (timestamp + this->getTimeDiff()));
}

void Device::sendCommand(uint8_t cmd, const void *args, uint8_t arglen) {
  uint16_t dat = arglen << 8 | cmd;
  write(this->cmd_fd, &dat, sizeof(uint16_t));

  if (args) {
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

  gettimeofday(&tp2, NULL);
  double ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000.0;

  double ms = ms2 - ms1;
  double time_diff = (ms1 + ms / 2) - mach_time;

  this->_time_diff->add(time_diff);

  printf("Device: %s\n", this->name);
  printf("---------------------------\n");

  printf("mach_time: %f\n", mach_time);
  printf("%f time diff (server - device)\n", time_diff);
  printf("%f ms elapsed\n", ms);
  printf("ms1: %f\nms2: %f\n", ms1, ms2);

  printf("%f avg diff\n", this->getTimeDiff());
  printf("\n");
}

void Device::startRecording() {
  printf("Start recording...\n");
  this->sendCommand(TCPDeviceCommandStartRecording, NULL, 0);
}

void Device::stopRecording() {
  this->sendCommand(TCPDeviceCommandStopRecording, NULL, 0);
}
void Device::upload() {
  this->sendCommand(TCPDeviceCommandUpload, NULL, 0);
}

void Device::setFileModeTCP() {
  printf("Setting mode to tcp...\n");
  this->sendCommand(TCPDeviceCommandFileModeTCP, NULL, 0);
}

void Device::dimScreen() {
  this->sendCommand(TCPDeviceCommandDimScreen, NULL, 0);
}
