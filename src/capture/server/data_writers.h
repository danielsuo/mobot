#include "Data.h"

#ifndef DATA_WRITERS_H
#define DATA_WRITERS_H

// Write files individually to disk
void disk_writer(Data *data);

// Write files as blob to disk (one big file)
void blob_writer(Data *data);

// Keep files in memeory
void mem_writer(Data *data);

#endif