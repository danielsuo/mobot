#ifndef MEMORYPARSER_H
#define MEMORYPARSER_H

#include "Parser/Parser.h"

class MemoryParser : public Parser {
public:
  MemoryParser(int index, char *name);

  virtual void preprocess() override;
  virtual void process() override;
  virtual void write(int data_length) override;
  virtual void postprocess() override;
};

#endif