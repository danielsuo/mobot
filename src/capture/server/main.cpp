/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 * TODO
 * ----
 * - Fix process quit if error on one of the threads (e.g., connection dead)
 *
 ******************************************************************************/

#include "StackTrace.h"
#include "TCPServer.h"
#include "Device.h"
#include "Data.h"

int main(int argc, char *argv[]) {

  StackTrace stackTrace;

#ifdef REWRITE
  FILE *fp;
  Data data;

  fp = fopen("gold", "r+");
  data.digest(fileno(fp));
  fp = fopen("silver", "r+");
  data.digest(fileno(fp));
  fp = fopen("blue", "r+");
  data.digest(fileno(fp));
  fp = fopen("pink", "r+");
  data.digest(fileno(fp));
#else
  TCPServer *server = new TCPServer(8124);

  server->add_device(new Device((char *)"gold", (char *)"192.168.0.105", 8124));
  server->add_device(new Device((char *)"silver", (char *)"192.168.0.106", 8124));
  server->add_device(new Device((char *)"blue", (char *)"192.168.0.107", 8124));
  server->add_device(new Device((char *)"pink", (char *)"192.168.0.108", 8124));
  // server->add_device(new Device((char *)"iPhone", (char *)"192.168.0.109", 8124));

  server->listen();
  server->connect();

  pthread_exit(NULL);
#endif

  return 0;
}