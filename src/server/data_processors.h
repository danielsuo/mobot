#include "Data.h"
#include "utilities.h"

#ifndef DATA_PROCESSORS_H
#define DATA_PROCESSORS_H

void disk_processor(Data *data);
void blob_processor(Data *data);

void disk_preprocessor(Data *data);
void blob_preprocessor(Data *data);

#endif