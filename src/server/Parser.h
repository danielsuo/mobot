/*******************************************************************************
 * server.cpp: C++ implementation of PVCapture TCP server
 *
 * Serial packet format
 * --------------------------------------------------------------
 * Description                     |    Variable | Length (bytes)
 * --------------------------------------------------------------
 * 1. File type (0: dir, 1: file)  |         N/A |              1
 * 2. Timestamp                    |         N/A |              8
 * 3. String length of file path   | PATH_LENGTH |              1
 * 4. File path                    |        N/A  |    PATH_LENGTH
 * 5. Byte length of file          | FILE_LENGTH |              4
 * 6. File contents                |        N/A  |    FILE_LENGTH
 *
 ******************************************************************************/

#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>

#ifdef DEBUG
// Check errors
#include <errno.h>

// Check system limits
#include <limits.h>
#endif

#include "Frame.h"
#include "Device.h"

#ifndef PARSER_H
#define PARSER_H

// Server buffer size to read each round
#define BUFFER_SIZE 4096

// Timeout period in seconds before server closes client connection
#define CONNECTION_TIMEOUT 5

// Get subarray
#define subarray(type, arr, off, len) (type (&)[len])(*(arr + off));

class Parser {
public:
  char    type;
  char *  path;
  char *  ext;
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

  // Length of path
  char path_length;

  // Metadata info
  int metadata_index;
  int metadata_length;

  // Buffer length and index
  uint32_t buffer_index;
  int buffer_length;

  // Not needed for all implementations of write
  FILE *fp;
  FILE *fp_timestamps;
  FILE *fp_filepaths;

  bool parsed;
  bool written;
  bool done;
  bool endOnEmptyBuffer;

  // Only needed for blob writing (e.g., latest timestamps)
  Device *device;

  // Use vector<char> because that's what cv::imdecode expects
  bool writing_color;
  bool writing_depth;
  vector<char> *color_buffer;
  vector<char> *depth_buffer;

  void (*preprocessor)(Parser *);
  void (*processor)(Parser *);
  void (*writer)(Parser *, bool);

  Parser();
  ~Parser();
  void digest(int fd);
  void show();

private:
  void preprocess();              // Process before entering read / process loop
  void read();                    // Read data from a buffer
  void parse();                   // Parse file metadata (once per file)
  void process();                 // Process file before writing
  void write(bool commit);        // Write file
  void clear();
};

#endif
