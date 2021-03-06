#include "TCPServer.h"

TCPServer::TCPServer(int port) {
  this->port = port;
  manager = new DeviceManager();

  struct timeval t;
  gettimeofday(&t, NULL);
  start_time = t.tv_sec * 1000 + t.tv_usec / 1000.0;
}

TCPServer::~TCPServer() {
  delete manager;
}

void *handler_client(void *server);

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
  for (int i = 0; i < manager->devices.size(); i++) {
    Device *device = manager->devices[i];
    int rc = pthread_create(&device->cmd_thread, NULL, handler_device, device);
    if (rc) {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}

void TCPServer::disconnect() {
  // First, stop recording
  for (int i = 0; i < manager->devices.size(); i++) {
    Device *device = manager->devices[i];
    fprintf(stderr, "Stop recording from device %d\n", i + 1);
    device->stopRecording();
  }

  // Wait to clean up
  sleep(2);

  // Next, clean up
  for (int i = 0; i < manager->devices.size(); i++) {
    Device *device = manager->devices[i];
    if (pthread_kill(device->cmd_thread, SIGKILL) == 0 && pthread_kill(device->dat_thread, SIGKILL) == 0) {
      fprintf(stderr, "Successfully killed threads from device %d\n", i + 1);
      if (close(device->dat_fd) > -1 && close(device->cmd_fd) > -1) {
        fprintf(stderr, "Successfully disconnected from device %d\n", i + 1);
      } else {
        perror("Failed to disconnect from device!");
      }
    } else {
      perror("Failed to kill threads");
    }
  }
}

void *handler_client_data(void *device_pointer) {
    Device *device = (Device *)device_pointer;

    device->parser->endOnEmptyBuffer = false;

    device->digest();

    pthread_exit(NULL);
    return 0;
}

void *handler_client(void *server) {
  TCPServer *self = (TCPServer *)server;

  // Stores size of the address of the client for the accept system call
  socklen_t clilen;

  // A sockaddr_in is a structure containing an internet address. This
  // structure is defined in netinet/in.h
  //
  // An in_addr structure, defined in the same header file, contains only one
  // field, a unsigned long called s_addr.
  //
  // struct sockaddr_in
  // {
  //   short   sin_family; /* must be AF_INET */
  //   u_short sin_port;
  //   struct  in_addr sin_addr;
  //   char    sin_zero[8]; /* Not used, must be zero */
  // };
  //
  // The variable serv_addr will contain the address of the server, and
  // cli_addr will contain the address of the client which connects to the
  // server.
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  // The socket() system call creates a new socket. There are three arguments:
  //
  // 1. The address domain of the socket: there are two possible address
  // domains, the unix domain for two processes which share a common file
  // system, and the Internet domain for any two hosts on the Internet. The
  // symbol constant AF_UNIX is used for the former, and AF_INET for the
  // latter (there are actually many other options which can be used here for
  // specialized purposes).
  //
  // 2. The second argument is the type of socket. Recall that there are two
  // choices here, a stream socket in which characters are read in a
  // continuous stream as if from a file or pipe, and a datagram socket, in
  // which messages are read in chunks. The two symbolic constants are
  // SOCK_STREAM and SOCK_DGRAM.
  //
  // 3. The third argument is the protocol. If this argument is zero (and it
  // always should be except for unusual circumstances), the operating system
  // will choose the most appropriate protocol. It will choose TCP for stream
  // sockets and UDP for datagram sockets.
  //
  // The socket system call returns an entry into the file descriptor table
  // (i.e. a small integer). This value is used for all subsequent references
  // to this socket.
  self->accept_socket = socket(AF_INET, SOCK_STREAM, 0);

  // the SO_REUSEADDR option tells the kernel that when the socket is closed,
  // the port bound to the socket should be freed immediately rather than kept
  // in-use for some period of time.
  int reuseaddr_option_val = 1;
  setsockopt(self->accept_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_option_val, sizeof(int));

  // If the socket call fails, it returns -1
  if (self->accept_socket < 0) {
    perror("ERROR opening socket");
  }

  // The function bzero() sets all values in a buffer to zero. It takes two
  // arguments, the first is a pointer to the buffer and the second is the
  // size of the buffer. Thus, this line initializes serv_addr to zeros.
  bzero((char *) &serv_addr, sizeof(serv_addr));

  // Address type is network
  serv_addr.sin_family = AF_INET;

  // Get port address in network byte order
  serv_addr.sin_port = htons(self->port);

  // The third field of sockaddr_in is a structure of type struct in_addr
  // which contains only a single field unsigned long s_addr. This field
  // contains the IP address of the host. For server code, this will always be
  // the IP address of the machine on which the server is running, and there
  // is a symbolic constant INADDR_ANY which gets this address.
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  // The bind() system call binds a socket to an address, in this case the
  // address of the current host and port number on which the server will run.
  // It takes three arguments, the socket file descriptor, the address to
  // which is bound, and the size of the address to which it is bound. The
  // second argument is a pointer to a structure of type sockaddr, but what is
  // passed in is a structure of type sockaddr_in, and so this must be cast to
  // the correct type. This can fail for a number of reasons, the most obvious
  // being that this socket is already in use on this machine.
  if (::bind(self->accept_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
  }
  else {
    printf("Starting server with pid %d at port %d\n", getpid(), self->port);
  }

  // The listen system call allows the process to listen on the socket for
  // connections. The first argument is the socket file descriptor, and the
  // second is the size of the backlog queue, i.e., the number of connections
  // that can be waiting while the process is handling a particular
  // connection. This should be set to 5, the maximum size permitted by most
  // systems.
  listen(self->accept_socket, 5);

  // Get length of client address
  clilen = sizeof(cli_addr);

  // Accept new connections and data until we kill the process
  while (1) {
    // The accept() system call causes the process to block until a client
    // connects to the server. Thus, it wakes up the process when a connection
    // from a client has been successfully established. It returns a new file
    // descriptor, and all communication on this connection should be done using
    // the new file descriptor. The second argument is a reference pointer to
    // the address of the client on the other end of the connection, and the
    // third argument is the size of this structure.
    int client_socket = accept(self->accept_socket, (struct sockaddr *) &cli_addr, &clilen);

    if (client_socket < 0) {
      perror("ERROR on accept");
    }

    printf("Received connection from %s\n", inet_ntoa(cli_addr.sin_addr));

    printf("Number of connected devices before: %lu\n", self->manager->devices.size());

    Device *device = self->manager->getDeviceByIPAddress(cli_addr.sin_addr.s_addr, cli_addr.sin_port);
    device->dat_fd = client_socket;

    printf("Number of connected devices: %lu\n", self->manager->devices.size());

    int rc = pthread_create(&device->dat_thread, NULL, handler_client_data, (void *)device);
    if (rc) {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }

    printf("\n");
  }

  // Close the sockets
  close(self->accept_socket);

  return 0;
}