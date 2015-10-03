#import "File.h"
#include "file_writers.h"
#include "file_processors.h"

File::File() {
  buffer = new char[BUFFER_SIZE];
  
  writer = disk_writer;
  processor = disk_processor;

  fp = NULL;

  clear();
}

File::~File() {
  if (buffer) free(buffer);
  if (path) free(path);
}

void File::digest(int fd) {
  this->fd = fd;

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
      fprintf(stderr, "TCP data connection timed out due to inactivity");
      break;
    }

    parsed = false;
    written = false;

    while (!written) {
      read();

      if (done) break;

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

void File::read() {
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
    done = true;
    fprintf(stderr, "Done!\n");
  }
}

void File::parse() {
  // Get file type
  type = buffer[buffer_index];
  buffer_index += sizeof(char);

  // Get timestamp
  double *timestamp_ptr = subarray(double, buffer, buffer_index, 1);
  received_timestamp = *timestamp_ptr;
  buffer_index += sizeof(double);

  // Get path length
  char path_length = buffer[buffer_index];
  buffer_index += sizeof(char);

  // Get file path
  path = substr(buffer, buffer_index, path_length);
  buffer_index += path_length;

  fprintf(stderr, "%s\n", path);

  // TODO: uncomment
  // if (timestamp > 0) device->processTimestamp(path, timestamp);

  // Get file length
  uint32_t *file_length_ptr = subarray(uint32_t, buffer, buffer_index, 1);
  file_length = *file_length_ptr;
  buffer_index += sizeof(uint32_t);

  parsed = true;
}

void File::process() {
  (*processor)(this);
}

// Check if directory exists first.
void File::write() {
  (*writer)(this);
}

void File::clear() {
  parsed = false;
  written = false;

  file_index = 0;
  file_length = 0;

  buffer_index = 0;
  buffer_length = 0;

  close(fd);
  if (fp != NULL) {
    fclose(fp);
    fp = NULL;
  }

  memset(buffer, 0, BUFFER_SIZE);
}