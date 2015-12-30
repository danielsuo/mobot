#include "Parser/DiskParser.h"

DiskParser::DiskParser(int index, char *name) : Parser(index, name) {}

void DiskParser::preprocess() {}

void DiskParser::process() {
  // If we're writing a directory, mkdir
  if (type == 0) {
    mkdirp(path, S_IRWXU, true);
  } else if (type == 1) {
    // Open file for appending bytes
    if (fp != NULL) {
      fclose(fp);
    }
    // TODO: we shouldn't need this
    mkdirp(path, S_IRWXU, false);
    fp = fopen(path, "ab");
  }
}

void DiskParser::write(int data_length) {
  fwrite(buffer + buffer_index, sizeof(char), data_length, fp);
}

void DiskParser::postprocess() {
  if (fp != NULL) {
    fclose(fp);
    fp = NULL;
  }
}