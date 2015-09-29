/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include "Device.h"
#include "TCPServer.h"

int main(int argc, char *argv[]) {

  TCPServer *server = new TCPServer(8124);

  // server->add_device(new Device((char *)"gold", (char *)"192.168.0.105", 8124));
  // server->add_device(new Device((char *)"silver", (char *)"192.168.0.106", 8124));
  // server->add_device(new Device((char *)"blue", (char *)"192.168.0.107", 8124));
  // server->add_device(new Device((char *)"pink", (char *)"192.168.0.108", 8124));
  server->add_device(new Device((char *)"iPhone", (char *)"192.168.0.109", 8124));

  server->listen();
  server->connect();

  pthread_exit(NULL);
  return 0;
}