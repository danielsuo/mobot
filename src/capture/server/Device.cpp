#include "Device.h"

Device::Device(char *name, char *host, unsigned int port) {
  this->name = name;
  this->host = host;
  this->port = port;

  this->num_frames_received = 0;

  this->_time_diff = new MovingAverage(AVERAGE_TIME_DIFF_OVER_NUM_PINGS);
}

Device::~Device() {
  close(this->cmd_fd);
}

void Device::connect() {
    struct hostent *server;
    struct sockaddr_in serv_addr;

    printf("Connecting to %s:%i\n", this->host, this->port);

    this->cmd_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->cmd_fd < 0) {
        perror("ERROR: opening socket");
    }

    server = gethostbyname(this->host);
    if (server == NULL) {
        perror("ERROR: no such host");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(this->port);
    if (::connect(this->cmd_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
    }
}

void Device::ping() {
    struct timeval tp1;
    struct timeval tp2;
    struct timeval tp3;

    this->connect();

    double remote_timestamp;
    printf("Socket %d:\n", this->cmd_fd);
    int n;

    while(1) {
        printf("Device: %s\n", this->name);
        printf("---------------------------\n");
        sleep(1);
        gettimeofday(&tp1, NULL);
        unsigned long ms1 = tp1.tv_sec * 1000 + tp1.tv_usec / 1000;

        n = write(this->cmd_fd, &ms1, sizeof(unsigned long));
        if (n < 0) 
             perror("ERROR writing to socket");
        // bzero(buffer, 256);
        n = read(this->cmd_fd, &remote_timestamp, 255);
        if (n < 0) 
             perror("ERROR reading from socket");
        gettimeofday(&tp2, NULL);

        printf("%f\n", remote_timestamp);

        unsigned long ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000;
        unsigned long ms3 = tp3.tv_sec * 1000 + tp3.tv_usec / 1000;
        unsigned long ms = ms2 - ms1;
        double time_diff = remote_timestamp - (ms1 + ms / 2.0);

        this->_time_diff->add(time_diff);

        printf("%f time diff (device - server)\n", time_diff);
        printf("%ld ms elapsed\n", ms);
        printf("ms1: %ld\nms2: %ld\nms3: %ld\n", ms1, ms2, ms3);
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