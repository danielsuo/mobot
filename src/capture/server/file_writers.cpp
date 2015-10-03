#include "file_writers.h"

// Write files individually to disk
void disk_writer(File *file) {
  int data_length = std::min(file->file_length - file->file_index, file->buffer_length - file->buffer_index);

  fwrite(file->buffer + file->buffer_index, sizeof(char), data_length, file->fp);
  file->buffer_index += data_length;
  file->file_index += data_length;

  file->written = file->file_index == file->file_length;

  if (file->written) {
    if (file->fp != NULL) {
      fclose(file->fp);
      file->fp = NULL;
    }
    file->file_index = 0;
    file->file_length = 0;
    fprintf(stderr, "Wrote to file %s\n", file->path);
  } else {
    memset(file->buffer, 0, BUFFER_SIZE);
  }
}

// Write files as blob to disk (one big file)
void blob_writer(File *file) {
  int data_length = std::min(file->file_length - file->file_index, file->buffer_length - file->buffer_index);

  fwrite(file->buffer + file->buffer_index, sizeof(char), data_length, file->fp);
  file->buffer_index += data_length;
  file->file_index += data_length;

  file->written = file->file_index == file->file_length;

  if (file->written) {
    file->file_index = 0;
    file->file_length = 0;
    fprintf(stderr, "Wrote to file %s\n", file->path);
  } else {
    memset(file->buffer, 0, BUFFER_SIZE);
  }
  
  // fwrite(file->buffer, sizeof(char), file->buffer_length, file->fp);
  
  // file->buffer_index = 0;
}

// Keep files in memeory
void mem_writer(File *file);