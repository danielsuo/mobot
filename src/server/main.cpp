/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include "TCPServer.h"
#include "DeviceManager.h"


int main(int argc, char *argv[]) {

  bool file = false;

  if (file) {
    DeviceManager *manager = new DeviceManager(DeviceOutputModeDisk);

    FILE *fp;
    fp = fopen("iPhone", "r+");
    manager->addDeviceByFileDescriptor((char *)"iPhone", fileno(fp));
    manager->digest();

    delete manager;
  } else {
    TCPServer *server = new TCPServer(8125, DeviceOutputModeMemory);
    DeviceManager *manager = server->manager;

  // manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124);
    manager->addDeviceByStringIPAddress((char *)"iPhone", (char *)"192.168.0.109", 8124);

    server->connect();
    server->listen();
    manager->runLoop();
  }

  pthread_exit(NULL);
}
