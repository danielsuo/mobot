#ifndef DISKPARSER_H
#define DISKPARSER_H

#include "Parser/Parser.h"

class DiskParser : public Parser {
public:
  DiskParser(int index, char *name);

  virtual void preprocess() override;
  virtual void process() override;
  virtual void write(int data_length) override;
  virtual void postprocess() override;
};

#endif