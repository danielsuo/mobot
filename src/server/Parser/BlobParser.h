#ifndef BLOBPARSER_H
#define BLOBPARSER_H

#include "Parser/Parser.h"

class BlobParser : public Parser {
public:
  BlobParser(int index, char *name);

  virtual void preprocess() override;
  virtual void process() override;
  virtual void write(int data_length) override;
  virtual void postprocess() override;
};

#endif