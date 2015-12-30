#ifndef MEMORYPARSER_H
#define MEMORYPARSER_H

#include "Parser/Parser.h"

class MemoryParser : public Parser {
public:
  MemoryParser(int index, char *name);

  void preprocess();
  void process();
  void write(int data_length);
  void postprocess();
};

#endif