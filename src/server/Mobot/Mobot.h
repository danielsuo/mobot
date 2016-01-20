#ifndef MOBOT_H
#define MOBOT_H

#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

using namespace std;

const char DRIVE = 'd';
const char TURN = 't';
const char ROTATE = 'r';

const int MOBOT_BUFFER_SIZE = 128;

class Mobot {
public:
  int fd;
  uint32_t addr;
  uint16_t port;
  char *buffer;
  int progress;

  Mobot(char *addr, uint16_t port);
  ~Mobot();

  void connect();
  void disconnect();
  void listen();
  void sendCommand(char cmd, int magnitude);

  void drive(int mm);
  void turn(int degrees);
  void rotate(int degrees);
};

#endif