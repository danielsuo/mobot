#include "file_processors.h"

void disk_preprocessor(File *file) {}

void blob_preprocessor(File *file) {
  // TODO: Move to preprocessor?
  file->fp = fopen(file->device->name, "ab");

  char fp_timestamps_filename[80] = {};
  strcpy(fp_timestamps_filename, file->device->name);
  strcat(fp_timestamps_filename, "-timestamps.txt");

  file->fp_timestamps = fopen(fp_timestamps_filename, "ab");
}

void disk_processor(File *file) {
  // If we're writing a directory, mkdir
  if (file->type == 0) {
    mkdirp(file->path, S_IRWXU, true);
  } else if (file->type == 1) {
    // Open file for appending bytes
    if (file->fp != NULL) {
      fclose(file->fp);
    }
    // TODO: we shouldn't need this
    mkdirp(file->path, S_IRWXU, false);
    file->fp = fopen(file->path, "ab");
  }
}

void blob_processor(File *file) {
  fwrite(file->buffer + file->metadata_index, sizeof(char), file->metadata_length, file->fp);

  char separators[2] = {' ', '\n'};

  file->timestamp = file->received_timestamp + file->device->getTimeDiff();
  fwrite(&file->timestamp, sizeof(double), 1, file->fp_timestamps);
  fwrite(separators, sizeof(char), 1, file->fp_timestamps);
  fwrite(file->path, sizeof(char), file->path_length, file->fp_timestamps);
  fwrite(separators + 1, sizeof(char), 1, file->fp_timestamps);
}
