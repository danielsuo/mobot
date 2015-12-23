#include "Parser.h"
#include "utilities.h"

#ifndef DATA_PROCESSORS_H
#define DATA_PROCESSORS_H

void disk_processor(Parser *parser);
void blob_processor(Parser *parser);
void memory_processor(Parser *parser);

void disk_preprocessor(Parser *parser);
void blob_preprocessor(Parser *parser);
void memory_preprocessor(Parser *parser);

#endif