#ifndef BLOBPARSER_H
#define BLOBPARSER_H

#include "Parser/Parser.h"

class BlobParser : public Parser {
public:
  BlobParser(int index, char *name);

  void preprocess();
  void process();
  void write(int data_length);
  void postprocess();
};

#endif