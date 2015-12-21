#include "data_processors.h"

void disk_preprocessor(Data *data) {}

void blob_preprocessor(Data *data) {
  // TODO: Move to preprocessor?
  data->fp = fopen(data->device->name, "ab");
  fprintf(stderr, "Preprocessing %s\n", data->device->name);

  char fp_timestamps_filename[80] = {};
  strcpy(fp_timestamps_filename, data->device->name);
  strcat(fp_timestamps_filename, "-timestamps.txt");

  char fp_filepaths_filename[80] = {};
  strcpy(fp_filepaths_filename, data->device->name);
  strcat(fp_filepaths_filename, "-filepaths.txt");

  data->fp_timestamps = fopen(fp_timestamps_filename, "ab");
  data->fp_filepaths = fopen(fp_filepaths_filename, "ab");
}

void memory_preprocessor(Data *data) {}

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
  fwrite(data->buffer + data->metadata_index, sizeof(char), data->metadata_length, data->fp);

  // Only write timestamp if it's color or depth image
  if (strcmp(data->ext, "jpg") == 0 || strcmp(data->ext, "png") == 0) {
    char newline = '\n';

    fprintf(stderr, "%f\n", data->device->getTimeDiff());

    data->timestamp = data->received_timestamp + data->device->getTimeDiff();
    fwrite(&data->timestamp, sizeof(double), 1, data->fp_timestamps);
    fwrite(data->path, sizeof(char), data->path_length, data->fp_timestamps);
    fwrite(&newline, sizeof(char), 1, data->fp_timestamps);

    fwrite(data->path, sizeof(char), data->path_length, data->fp_filepaths);
    fwrite(&newline, sizeof(char), 1, data->fp_filepaths);

  }
}

void memory_processor(Data *data) {
  if (data->type == 1) {

    // If we have a jpg, create a new frame
    if (strcmp(data->ext, "jpg") == 0) {
      data->color_buffer = new vector<char>();
      data->color_buffer->reserve(data->file_length);

      data->writing_color = true;

      // Create frame and pair objects
      Frame *frame = new Frame(data->parameters);
      frame->index = data->frames.size();
      data->frames.push_back(frame);
    }

    // Add to the existing frame
    else if (strcmp(data->ext, "png") == 0) {
      data->depth_buffer = new vector<char>();
      data->depth_buffer->reserve(data->file_length);
      data->writing_depth = true;
    }

    // For now, ignore everything else
  }
}