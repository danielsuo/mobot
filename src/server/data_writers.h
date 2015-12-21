#include "Data.h"

#ifndef DATA_WRITERS_H
#define DATA_WRITERS_H

// Write files individually to disk
void disk_writer(Data *data, bool commit);

// Write files as blob to disk (one big file)
void blob_writer(Data *data, bool commit);

// Keep files in memeory
void memory_writer(Data *data, bool commit);

#endif