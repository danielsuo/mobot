#include "File.h"
#include "utilities.h"

#ifndef FILE_PROCESSORS_H
#define FILE_PROCESSORS_H

void disk_processor(File *file);
void blob_processor(File *file);

void disk_preprocessor(File *file);
void blob_preprocessor(File *file);

#endif