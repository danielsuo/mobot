#import "File.h"

// TODO
File::File() {
  parsed = false;
  written = false;

  file_index = 0;
  file_length = 0;

  buffer_index = 0;
  buffer_length = 0;

  if (!buffer) buffer = new char[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
}

File::~File() {
  if (buffer) free(buffer);
  if (path) free(path);
}

// Some helper functions
// ---------------------

// Get character substrings
char *substr(char *arr, int begin, int len)
{
  char *res = (char *)malloc(sizeof(char) * len);
  for (int i = 0; i < len; i++)
    res[i] = *(arr + begin + i);
  res[len] = 0;

  return res;
}

// Create file directories recursively
void mkdirp(char *dir, mode_t mode, bool is_dir) {

  // Find the string length of the directory path
  int len = 0;
  while (*(dir + len) != 0) len++; len++;

  char *p = NULL;

  // Remove any trailing /
  if (dir[len - 1] == '/') {
    dir[len - 1] = 0;
  }

  struct stat st = {0};

  // Loop through each character in the directory path
  for (p = dir + 1; *p; p++) {

    // If the character is /, temporarily replace with \0 to terminate
    // string and create directory at the parent path
    if (*p == '/') {

      *p = 0;

      if (stat(dir, &st) == -1) {
        mkdir(dir, mode);
      }

      // Change \0 back to /
      *p = '/';
    }
  }

  // Create the last directory in the hierarchy
  if (stat(dir, &st) == -1 && is_dir) {
    mkdir(dir, mode);
  }
}

void File::digest(int fd) {
  this->fd = fd;
  clear();


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
      // fprintf(stderr, "Device: %s | Timed out from no data.\n", "device->name");
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
      }

      write();
    }
  }
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

  // TODO: uncomment
  // if (timestamp > 0) device->processTimestamp(path, timestamp);

  // Get file length
  uint32_t *file_length_ptr = subarray(uint32_t, buffer, buffer_index, 1);
  file_length = *file_length_ptr;
  buffer_index += sizeof(uint32_t);

  parsed = true;

  // If we're writing a directory, mkdir
  if (type == 0) {
    mkdirp(path, S_IRWXU, true);
  } else if (type == 1) {
    // Open file for appending bytes
    if (fp != NULL) {
      fclose(fp);
    }
    // TODO: we shouldn't need this
    mkdirp(path, S_IRWXU, false);
    fp = fopen(path, "ab");
  }
  if (fp == NULL) {
    fprintf(stderr, "Path: %s | Outfile null...\n", "device->name");
  } else {
    fprintf(stderr, "Path: %s | %p\n", path, (void *)fp);
  }
  // char filePath[PATH_MAX];
  // int fno;
  // fno = fileno(fp);
  // fprintf(stderr, "Device: %s | fno: %d\n", "device->name", fno);
  // if (fcntl(fno, F_GETPATH, filePath) != -1) {
  //   fprintf(stderr, "Device: %s | Outfile exists at %s\n", "device->name", filePath);
  // } else {
  //   fprintf(stderr, "Device: %s | Outfile doesn't exist!\n", "device->name");
  // }
}

// Check if directory exists first.
void File::write() {
  int data_length = std::min(file_length - file_index, buffer_length - buffer_index);

  fwrite(buffer + buffer_index, sizeof(char), data_length, fp);
  buffer_index += data_length;
  file_index += data_length;

  written = file_index == file_length;
  if (written)
  {
    if (fp != NULL) {
      fclose(fp);
      fp = NULL;
    }
    file_index = 0;
    file_length = 0;
    fprintf(stderr, "Wrote to file %s\n", path);
  } else {
    memset(buffer, 0, BUFFER_SIZE);
  }
}

void File::clear() {

}