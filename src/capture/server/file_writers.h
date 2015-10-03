#include "File.h"

#ifndef FILE_WRITERS_H
#define FILE_WRITERS_H

// Write files individually to disk
void disk_writer(File *file);

// Write files as blob to disk (one big file)
void blob_writer(File *file);

// Keep files in memeory
void mem_writer(File *file);

#endif