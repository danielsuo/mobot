#include "Parser/Parser.h"
#include "Parser/MemoryParser.h"
#include "Parser/DiskParser.h"
#include "Parser/BlobParser.h"

Parser *Parser::createParser(int index, char *name, ParserOutputMode mode) {
  switch(mode) {
    case ParserOutputModeBlob:
      return new BlobParser(index, name);
    break;

    case ParserOutputModeDisk:
      return new DiskParser(index, name);
    break;

    case ParserOutputModeMemory:
      return new MemoryParser(index, name);
    break;
  }
}

Parser::Parser(int index, char *name) {
  this->mode = mode;
  this->name = name;
  this->index = index;

  out_dir = "./";

  buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));

  fp = NULL;
  fp_timestamps = NULL;
  fp_filepaths = NULL;
  endOnEmptyBuffer = true;

  writing_color = false;
  writing_depth = false;

  queue = new ReaderWriterQueue<Pair *>(1000);
  queue_length = 0;

  clear();
}

Parser::~Parser() {
  free(buffer);

  Pair *pair;
  while (queue->try_dequeue(pair)) {
    delete pair;
  }
  delete queue;
}

void Parser::digest(int fd) {
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
    if (select(fd + 1, &readfds, NULL, NULL, &tv) < 0) {
      perror("ERROR: could not select");
    }

    // If we haven't had a read within our timeout, return from this
    // function and close the connection
    if (!FD_ISSET(fd, &readfds)) {
      fprintf(stderr, "File descriptor %d timed out due to inactivity\n", fd);
      break;
    }

    parsed = false;
    written = false;
    bool commit = true;

    // While we haven't finished reading the file, loop
    while (!written) {

      // Read next chunk into buffer
      read();

      // If there is no more data, break
      if (done) break;

      // If we have an error reading, try again
      if (buffer_length <= 0) continue;

      // If we haven't parsed the metadata yet, parse
      if (!parsed) {

        // Assumes buffer is large enough to hold metadata
        parse();

        // Only commit data to write if the device is ready to record (i.e.,
        // we have correct timestamp) or we have a non-image file to write
        commit = readyToRecord || (strcmp(ext, "jpg") != 0 && strcmp(ext, "png") != 0 && strcmp(ext, "ION") != 0);

        if (commit) process();
      }

      // Check how much data we're going to write
      int data_length = std::min(file_length - file_index, buffer_length - buffer_index);

      if (commit) write(data_length);

      // Update our place in the buffer and the file
      buffer_index += data_length;
      file_index += data_length;

      written = file_index == file_length;

      if (written) {
        free(path);
        file_index = 0;
        file_length = 0;

        // Perform any post-processing
        if (commit) postprocess();
      } else {
        // Reset the buffer
        memset(buffer, 0, BUFFER_SIZE);
      }
    }
  }

  clear();
}

void Parser::read() {
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
  } else {
    // fprintf(stderr, "Reading %d bytes of data", buffer_length);
  }
}

void Parser::parse() {
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

  // Get file extension
  ext = path + path_length - 3;

  // Get file length
  uint32_t *file_length_ptr = subarray(uint32_t, buffer, buffer_index, 1);
  file_length = *file_length_ptr;
  buffer_index += sizeof(uint32_t);

  metadata_length = buffer_index - metadata_index;

  // TODO: Shame unto those who hath wrought such foulness
  char *file_path = path;
  path = (char *)malloc(sizeof(char) * 1000);
  sprintf(path, "%s/%s", out_dir.c_str(), file_path);
  free(file_path);

  parsed = true;
}

void Parser::clear() {
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

  if (fp_filepaths != NULL) {
    fclose(fp_filepaths);
    fp_filepaths = NULL;
  }

  memset(buffer, 0, BUFFER_SIZE);
}

void Parser::show() {
  fprintf(stderr, "Parser at path %s with type %d\n", path, type);

  if (fp == NULL) {
    fprintf(stderr, "Parser file pointer null\n");
  }
}

void Parser::preprocess() {}
void Parser::process() {}
void Parser::write(int data_length) {}
void Parser::postprocess() {}
