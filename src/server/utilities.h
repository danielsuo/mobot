#include <stdlib.h>
#include <stdio.h>

// Manipulate file descriptors
#include <fcntl.h>

// Check system limits
#include <limits.h>

// Check if directory exists
#include <sys/stat.h>

#ifndef UTILITES_H
#define UTILITES_H

// Get character substrings
char *substr(char *arr, int begin, int len);

// Create file directories recursively
void mkdirp(char *dir, mode_t mode, bool is_dir);

// Print pathname from file pointer
void print_pathname_from_file_pointer(FILE *fp);

// void write_plys(const char *plyfile, const vector<Frames *>frames);

#endif