#include "Parser/BlobParser.h"

BlobParser::BlobParser(int index, char *name) : Parser(index, name) {}

void BlobParser::preprocess() {
  fp = fopen(name, "ab");
  fprintf(stderr, "Preprocessing %s\n", name);

  char fp_timestamps_filename[80] = {};
  strcpy(fp_timestamps_filename, name);
  strcat(fp_timestamps_filename, "-timestamps.txt");

  char fp_filepaths_filename[80] = {};
  strcpy(fp_filepaths_filename, name);
  strcat(fp_filepaths_filename, "-filepaths.txt");

  fp_timestamps = fopen(fp_timestamps_filename, "ab");
  fp_filepaths = fopen(fp_filepaths_filename, "ab");
}

void BlobParser::process() {
  fwrite(buffer + metadata_index, sizeof(char), metadata_length, fp);

  // Only write timestamp if it's color or depth image
  if (strcmp(ext, "jpg") == 0 || strcmp(ext, "png") == 0) {
    char newline = '\n';

    timestamp = received_timestamp + time_diff;
    fwrite(&timestamp, sizeof(double), 1, fp_timestamps);
    fwrite(path, sizeof(char), path_length, fp_timestamps);
    fwrite(&newline, sizeof(char), 1, fp_timestamps);

    fwrite(path, sizeof(char), path_length, fp_filepaths);
    fwrite(&newline, sizeof(char), 1, fp_filepaths);

  }
}

void BlobParser::write(int data_length) {
  fwrite(buffer + buffer_index, sizeof(char), data_length, fp);
}

void BlobParser::postprocess() {}