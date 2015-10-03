#include "file_processors.h"

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
  file->fp = fopen(file->device->name, "ab");

  file->timestamp = file->device->getTimeDiff();
  
}