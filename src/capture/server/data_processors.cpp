#include "data_processors.h"

void disk_preprocessor(Data *data) {}

void blob_preprocessor(Data *data) {
  // TODO: Move to preprocessor?
  data->fp = fopen(data->device->name, "ab");
  fprintf(stderr, "Preprocessing %s\n", data->device->name);
  print_pathname_from_file_pointer(data->fp);

  char fp_timestamps_filename[80] = {};
  strcpy(fp_timestamps_filename, data->device->name);
  strcat(fp_timestamps_filename, "-timestamps.txt");

  data->fp_timestamps = fopen(fp_timestamps_filename, "ab");
  // fclose(tmp);
  // tmp = NULL;
}

void disk_processor(Data *data) {
  // If we're writing a directory, mkdir
  if (data->type == 0) {
    mkdirp(data->path, S_IRWXU, true);
  } else if (data->type == 1) {
    // Open file for appending bytes
    if (data->fp != NULL) {
      fclose(data->fp);
    }
    // TODO: we shouldn't need this
    mkdirp(data->path, S_IRWXU, false);
    data->fp = fopen(data->path, "ab");
  }
}

void blob_processor(Data *data) {
  fprintf(stderr, "Processing %s\n", data->device->name);
  print_pathname_from_file_pointer(data->fp);
  print_pathname_from_file_pointer(data->fp_timestamps);

  fwrite(data->buffer + data->metadata_index, sizeof(char), data->metadata_length, data->fp);

  char separators[2] = {' ', '\n'};

  data->timestamp = data->received_timestamp + data->device->getTimeDiff();
  fwrite(&data->timestamp, sizeof(double), 1, data->fp_timestamps);
  fwrite(separators, sizeof(char), 1, data->fp_timestamps);
  fwrite(data->path, sizeof(char), data->path_length, data->fp_timestamps);
  fwrite(separators + 1, sizeof(char), 1, data->fp_timestamps);
}
