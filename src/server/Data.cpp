#include "Data.h"
#include "data_writers.h"
#include "data_processors.h"

Data::Data() {
  buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
  
  preprocessor = disk_preprocessor;
  processor = disk_processor;
  writer = disk_writer;

  fp = NULL;
  fp_timestamps = NULL;
  endOnEmptyBuffer = true;

  writing_color = false;
  writing_depth = false;

  clear();
}

Data::~Data() {
  for (unsigned int i = 0; i < frames.size(); i++) {
    delete frames[i];
  }

  free(buffer);
}

void Data::digest(int fd) {
  this->fd = fd;

  preprocess();

  while (!done) {
    // Create file descriptor set so we can check which descriptors have
    // reads available
    fd_set readfds;

    // Create a struct to hold the connection timeout length
    struct timeval tv;

    // Give connection timeout in seconds and microseconds
    tv.tv_sec = CONNECTION_TIMEOUT;
    tv.tv_usec = 0;

    // Set reads available to false for all file descriptors except the
    // current socket. We only want to track our socket
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    // don't care about writefds and exceptfds:
    select(fd + 1, &readfds, NULL, NULL, &tv);

    // If we haven't had a read within our timeout, return from this
    // function and close the connection
    if (!FD_ISSET(fd, &readfds)) {
      fprintf(stderr, "TCP data connection timed out due to inactivity\n");
      break;
    }

    parsed = false;
    written = false;

    while (!written) {
      read();

      if (done) break;

      if (buffer_length <= 0) continue;

      if (!parsed) {
        // Assumes buffer is large enough to hold metadata
        parse();
        process();
      }

      write();
    }
  }

  clear();
}

void Data::read() {
  // Buffer currently has some data
  if (buffer_index > 0) {
    // Shuffle existing buffer data forward
    for (int i = buffer_index; i < buffer_length; i++) {
      buffer[i - buffer_index] = buffer[i];
    }

    // Zero out remaining buffer
    buffer_index = buffer_length - buffer_index;
    memset(buffer + buffer_index, 0, buffer_length - buffer_index);
  }

  // Read new data and reset buffer_index to 0
  buffer_length = buffer_index + ::read(fd, buffer + buffer_index, BUFFER_SIZE - buffer_index - 1);
  buffer_index = 0;

  if (buffer_length <= 0) {
    done = endOnEmptyBuffer;
    if (endOnEmptyBuffer) {
      fprintf(stderr, "Done!\n");
    }
  }
}

void Data::parse() {
  metadata_index = buffer_index;

  // Get file type
  type = buffer[buffer_index];
  buffer_index += sizeof(char);

  // Get timestamp
  double *timestamp_ptr = subarray(double, buffer, buffer_index, 1);
  received_timestamp = *timestamp_ptr;
  buffer_index += sizeof(double);

  // Get path length
  path_length = buffer[buffer_index];
  buffer_index += sizeof(char);

  // Get file path
  path = substr(buffer, buffer_index, path_length);
  buffer_index += path_length;

  // Get file length
  uint32_t *file_length_ptr = subarray(uint32_t, buffer, buffer_index, 1);
  file_length = *file_length_ptr;
  buffer_index += sizeof(uint32_t);

  metadata_length = buffer_index - metadata_index;

  parsed = true;
}

void Data::preprocess() {
  (*preprocessor)(this);
}

void Data::process() {
  (*processor)(this);
}

// Check if directory exists first.
void Data::write() {
  (*writer)(this);
}

void Data::clear() {
  parsed = false;
  written = false;

  file_index = 0;
  file_length = 0;

  buffer_index = 0;
  buffer_length = 0;

  // delete color_buffer;
  // delete depth_buffer;

  done = false;

  close(fd);

  if (fp != NULL) {
    fclose(fp);
    fp = NULL;
  }

  if (fp_timestamps != NULL) {
    fclose(fp_timestamps);
    fp_timestamps = NULL;
  }

  memset(buffer, 0, BUFFER_SIZE);
}

void Data::show() {
  fprintf(stderr, "Data at path %s with type %d\n", path, type);

  if (fp == NULL) {
    fprintf(stderr, "Data file pointer null\n");
  }
}