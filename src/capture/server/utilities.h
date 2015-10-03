#include <stdlib.h>

// Check if directory exists
#include <sys/stat.h>

#ifndef UTILITES_H
#define UTILITES_H

// Get character substrings
char *substr(char *arr, int begin, int len);

// Create file directories recursively
void mkdirp(char *dir, mode_t mode, bool is_dir);

#endif