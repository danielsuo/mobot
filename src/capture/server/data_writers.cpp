#include "data_writers.h"

// Write files individually to disk
void disk_writer(Data *data) {
  int data_length = std::min(data->file_length - data->file_index, data->buffer_length - data->buffer_index);

  fwrite(data->buffer + data->buffer_index, sizeof(char), data_length, data->fp);
  data->buffer_index += data_length;
  data->file_index += data_length;

  data->written = data->file_index == data->file_length;

  if (data->written) {
    if (data->fp != NULL) {
      fclose(data->fp);
      data->fp = NULL;
    }
    data->file_index = 0;
    data->file_length = 0;
    fprintf(stderr, "Wrote to file %s\n", data->path);
  } else {
    memset(data->buffer, 0, BUFFER_SIZE);
  }
}

// Write files as blob to disk (one big file)
void blob_writer(Data *data) {

  int data_length = std::min(data->file_length - data->file_index, data->buffer_length - data->buffer_index);

  fwrite(data->buffer + data->buffer_index, sizeof(char), data_length, data->fp);
  data->buffer_index += data_length;
  data->file_index += data_length;

  data->written = data->file_index == data->file_length;

  if (data->written) {
    data->file_index = 0;
    data->file_length = 0;
    fprintf(stderr, "Wrote to file %s\n", data->path);
  } else {
    memset(data->buffer, 0, BUFFER_SIZE);
  }
}

// Keep files in memeory
void mem_writer(Data *data);