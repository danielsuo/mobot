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

#ifndef PARSER_H
#define PARSER_H

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

#include "readerwriterqueue/readerwriterqueue.h"

#include "Pair.h"
#include "Frame.h"

// Server buffer size to read each round
const int BUFFER_SIZE = 4096;

// Timeout period in seconds before server closes client connection
const int CONNECTION_TIMEOUT = 5;

// Get subarray
#define subarray(type, arr, off, len) (type (&)[len])(*(arr + off));

using namespace moodycamel;

typedef enum {
  ParserOutputModeBlob,
  ParserOutputModeDisk,
  ParserOutputModeMemory
} ParserOutputMode;

class Parser {
public:
  int     index;
  char *  name;

  ParserOutputMode mode;

  char    type;
  char *  path;
  char *  ext;
  double  timestamp;
  double  received_timestamp;
  double  time_diff;

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
  bool readyToRecord;
  bool endOnEmptyBuffer;

  // Store data
  ReaderWriterQueue<Pair *> *queue;
  int queue_length;

  // Use vector<char> because that's what cv::imdecode expects
  bool writing_color;
  bool writing_depth;
  vector<char> *color_buffer;
  vector<char> *depth_buffer;

  Parser(int index, char *name);
  ~Parser();
  void digest(int fd);
  void show();

protected:
  void read();    // Read data from a buffer
  void parse();   // Parse file metadata (once per file)
  void clear();
  
  virtual void preprocess();                          // Process before entering read / process loop
  virtual void process();                             // Process file before writing
  virtual void write(int data_length);                // Write file
  virtual void postprocess();
};

class ParserFactory {
public:
  static Parser *createParser(int index, char *name, ParserOutputMode mode);
};

#endif
