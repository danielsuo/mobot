#include "utilities.h"

char *substr(char *arr, int begin, int len) {
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
