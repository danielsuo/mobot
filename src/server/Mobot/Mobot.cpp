#include "Mobot/Mobot.h"

void *handler(void *self);

Mobot::Mobot(char *addr, uint16_t port) {
  this->addr = inet_addr(addr);
  this->port = port;

  buffer = (char *)malloc(MOBOT_BUFFER_SIZE * sizeof(char));
}

Mobot::~Mobot() {
  free(buffer);
}

void Mobot::connect() {
  struct sockaddr_in serv_addr;
  fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd < 0) {
    perror("ERROR: can't get file descriptor to Mobot!");
    exit(-1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = addr;

  serv_addr.sin_port = htons(port);
  int result = ::connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

  if (result < 0) {
    perror("Error: could not connect");
  }
}

void Mobot::listen() {
  pthread_t listener;
  printf("Listening on %d\n", port);
  fcntl(fd, F_SETFL, O_NONBLOCK);
  int rc = pthread_create(&listener, NULL, handler, this);
  if (rc) {
    fprintf(stderr, "ERROR: return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
}

void Mobot::disconnect() {
  if (close(fd) < 0) {
    perror("ERROR: couldn't close connection to Mobot");
    exit(-1);
  } else {
    fprintf(stderr, "Disconnecting from Mobot\n");
    fd = -1;
  }
}

void Mobot::sendCommand(char cmd, int magnitude) {
  if (progress > 0 && progress < 100) {
    fprintf(stderr, "ERROR: Mobot is currently moving and is %d%% of the way there\n", progress);
    return;
  }

  progress = 0;

  string data = cmd + to_string(magnitude) + '\n';
  const char *to_send = data.c_str();
  fprintf(stderr, "Sent data: %s", to_send);
  write(fd, to_send, data.length());
}

void Mobot::drive(int mm) {
  sendCommand(DRIVE, mm);
}

void Mobot::turn(int degrees) {
  sendCommand(TURN, degrees);
}

void Mobot::rotate(int degrees) {
  sendCommand(ROTATE, degrees);
}

void *handler(void *self) {
  Mobot *mobot = (Mobot *) self;

  while (true) {
    int data_length = read(mobot->fd, mobot->buffer, MOBOT_BUFFER_SIZE - 1);

    if (data_length > 0) {
      fprintf(stderr, "Received data: %s\n", mobot->buffer);
      mobot->progress = atoi(mobot->buffer);
      fprintf(stderr, "Current progress: %d\n", mobot->progress);
      bzero(mobot->buffer, MOBOT_BUFFER_SIZE);
    } else if (errno != EAGAIN) {
      perror("Error");
      break;
    } 

    usleep(1000);
  }

  fprintf(stderr, "Connection closed, stop listening\n");

  pthread_exit(NULL);
}