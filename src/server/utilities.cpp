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

void composeTransform(float *A, float *B, float *out) {
  out[ 0] = A[0] * B[0] + A[1] * B[4] + A[2] * B[8];
  out[ 1] = A[0] * B[1] + A[1] * B[5] + A[2] * B[9];
  out[ 2] = A[0] * B[2] + A[1] * B[6] + A[2] * B[10];
  out[ 3] = A[0] * B[3] + A[1] * B[7] + A[2] * B[11] + A[3];
  out[ 4] = A[4] * B[0] + A[5] * B[4] + A[6] * B[8];
  out[ 5] = A[4] * B[1] + A[5] * B[5] + A[6] * B[9];
  out[ 6] = A[4] * B[2] + A[5] * B[6] + A[6] * B[10];
  out[ 7] = A[4] * B[3] + A[5] * B[7] + A[6] * B[11] + A[7];
  out[ 8] = A[8] * B[0] + A[9] * B[4] + A[10] * B[8];
  out[ 9] = A[8] * B[1] + A[9] * B[5] + A[10] * B[9];
  out[10] = A[8] * B[2] + A[9] * B[6] + A[10] * B[10];
  out[11] = A[8] * B[3] + A[9] * B[7] + A[10] * B[11] + A[11];
}