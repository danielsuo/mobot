#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

class File {
public:
  char    type;
  char *  path;
  double  timestamp;
  double  received_timestamp;

  // File descriptor of read location
  int fd;

  // Buffer that stores data
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

  bool parsed;
  bool written;
  bool done;

  Device *device;

  void (*processor)(File *);
  void (*writer)(File *);

  File();
  ~File();
  void digest(int fd);

private:
  void read();      // Read data from a buffer
  void parse();     // Parse file metadata (once per file)
  void process();   // Process file before writing
  void write();     // Write file
  void clear();
  
};

#endif