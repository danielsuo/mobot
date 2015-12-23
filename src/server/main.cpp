/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include "TCPServer.h"
#include "DeviceManager.h"


int main(int argc, char *argv[]) {

  TCPServer *server = new TCPServer(8124, DeviceOutputModeBlob);
  DeviceManager *manager = server->manager;

  // manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124);
  manager->addDeviceByStringIPAddress((char *)"iPhone", (char *)"192.168.0.109", 8124);

  server->connect();
  server->listen();

  // DeviceManager *manager = new DeviceManager(DeviceOutputModeDisk);

  // FILE *fp;
  // fp = fopen("device1", "r+");
  // manager->addDeviceByFileDescriptor((char *)"device1", fileno(fp));
  // manager->digest();

  // delete manager;

  pthread_exit(NULL);
}

// #ifdef REWRITE
//   FILE *fp;
//   Parser *parser = new Parser();

//   #ifdef MEMORY
//   // Parameters *parameters = new Parameters(argv[1], argv[2]);
//   Parameters *parameters = new Parameters("../", "data/");

//   fp = fopen("iPhone", "r+");
  
//   parser->preprocessor = memory_preprocessor;
//   parser->processor = memory_processor;
//   parser->writer = memory_writer;

//   parser->parameters = parameters;
//   parser->digest(fileno(fp));
  
//   #elif DISK

//   parser->preprocessor = disk_preprocessor;
//   parser->processor = disk_processor;
//   parser->writer = disk_writer;

//   // gold
//   fp = fopen("device3", "r+");
//   parser->digest(fileno(fp));

//   // silver
//   fp = fopen("device2", "r+");
//   parser->digest(fileno(fp));

//   // blue
//   fp = fopen("device4", "r+");
//   parser->digest(fileno(fp));

//   // pink
//   fp = fopen("device1", "r+");
//   parser->digest(fileno(fp));

//   // fp = fopen("iPhone", "r+");
//   // parser->digest(fileno(fp));

//   #endif


//   delete parser;
//   fclose(fp);
//   fp = NULL;

// #else
//   TCPServer *server = new TCPServer(8124);

//   server->add_device(new Device((char *)"device3", (char *)"192.168.0.105", 8124));
//   server->add_device(new Device((char *)"device2", (char *)"192.168.0.106", 8124));
//   server->add_device(new Device((char *)"device4", (char *)"192.168.0.107", 8124));
//   server->add_device(new Device((char *)"device1", (char *)"192.168.0.108", 8124));
//   // server->add_device(new Device((char *)"iPhone", (char *)"192.168.0.109", 8124));

//   server->listen();
//   server->connect();

//   pthread_exit(NULL);
// #endif