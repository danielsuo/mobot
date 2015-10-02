#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Check if directory exists
#include <sys/stat.h>

#ifdef DEBUG
// Check errors
#include <errno.h>

// Check system limits
#include <sys/syslimits.h>
#endif

#include "Device.h"

#ifndef File_H
#define File_H

// Server buffer size to read each round
#define BUFFER_SIZE 4096

// Timeout period in seconds before server closes client connection
#define CONNECTION_TIMEOUT 5

// Get subarray
#define subarray(type, arr, off, len) (type (&)[len])(*(arr + off));

// TODO: concurrent queue
// - https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/
// - http://blogs.msmvps.com/vandooren/2007/01/05/creating-a-thread-safe-producer-consumer-queue-in-c-without-using-locks/

class File {
public:
  char    type;
  char *  path;
  double  timestamp;
  double  received_timestamp;

  bool parsed;
  bool written;
  bool done;

  Device *device;

  // TODO add function pointers
  // void (*process)(void *);

  File();
  ~File();
  void digest(int fd);

private:
  // File descriptor of read location
  int fd;

  // 
  char *buffer;

  // Byte index in the current file
  uint32_t file_index;

  // Byte syze of the current file
  uint32_t file_length;

  // Buffer length and index
  uint32_t buffer_index;
  int buffer_length;

  // Not needed for all implementations of write
  FILE *fp;

  void read();
  void parse();
  void write();
  void clear();
  
};

#endif