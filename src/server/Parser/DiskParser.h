#ifndef DISKPARSER_H
#define DISKPARSER_H

#include "Parser/Parser.h"

class DiskParser : public Parser {
public:
  DiskParser(int index, char *name);

  void preprocess();
  void process();
  void write(int data_length);
  void postprocess();
};

#endif