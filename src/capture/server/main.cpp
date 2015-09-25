/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include <stdlib.h>
#include <pthread.h>

#include "Device.h"

#define NUM_DEVICES     4
Device *devices[NUM_DEVICES];
pthread_t threads[NUM_DEVICES];

void *handler(void *device_pointer) {
  Device *device = (Device *)device_pointer;

  device->ping();

  return 0;
}

int main(int argc, char *argv[]) {

    devices[0] = new Device((char *)"gold", (char *)"192.168.0.105", 8124);
    devices[1] = new Device((char *)"silver", (char *)"192.168.0.106", 8124);
    devices[2] = new Device((char *)"blue", (char *)"192.168.0.107", 8124);
    devices[3] = new Device((char *)"pink", (char *)"192.168.0.108", 8124);

    for (int i = 0; i < NUM_DEVICES; i++) {
        int rc = pthread_create(&threads[0], NULL, handler, devices[i]);
        if (rc) {
          printf("ERROR: return code from pthread_create() is %d\n", rc);
          exit(-1);
        }
    }

    pthread_exit(NULL);

    return 0;
}