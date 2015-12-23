#include "data_processors.h"

void disk_preprocessor(Parser *parser) {}

void blob_preprocessor(Parser *parser) {
  // TODO: Move to preprocessor?
  parser->fp = fopen(parser->device->name, "ab");
  fprintf(stderr, "Preprocessing %s\n", parser->device->name);

  char fp_timestamps_filename[80] = {};
  strcpy(fp_timestamps_filename, parser->device->name);
  strcat(fp_timestamps_filename, "-timestamps.txt");

  char fp_filepaths_filename[80] = {};
  strcpy(fp_filepaths_filename, parser->device->name);
  strcat(fp_filepaths_filename, "-filepaths.txt");

  parser->fp_timestamps = fopen(fp_timestamps_filename, "ab");
  parser->fp_filepaths = fopen(fp_filepaths_filename, "ab");
}

void memory_preprocessor(Parser *parser) {}

void disk_processor(Parser *parser) {
  // If we're writing a directory, mkdir
  if (parser->type == 0) {
    mkdirp(parser->path, S_IRWXU, true);
  } else if (parser->type == 1) {
    // Open file for appending bytes
    if (parser->fp != NULL) {
      fclose(parser->fp);
    }
    // TODO: we shouldn't need this
    mkdirp(parser->path, S_IRWXU, false);
    parser->fp = fopen(parser->path, "ab");
  }
}

void blob_processor(Parser *parser) {
  fprintf(stderr, "Processing %s\n", parser->device->name);
  fwrite(parser->buffer + parser->metadata_index, sizeof(char), parser->metadata_length, parser->fp);

  // Only write timestamp if it's color or depth image
  if (strcmp(parser->ext, "jpg") == 0 || strcmp(parser->ext, "png") == 0) {
    char newline = '\n';

    fprintf(stderr, "%f\n", parser->device->getTimeDiff());

    parser->timestamp = parser->received_timestamp + parser->device->getTimeDiff();
    fwrite(&parser->timestamp, sizeof(double), 1, parser->fp_timestamps);
    fwrite(parser->path, sizeof(char), parser->path_length, parser->fp_timestamps);
    fwrite(&newline, sizeof(char), 1, parser->fp_timestamps);

    fwrite(parser->path, sizeof(char), parser->path_length, parser->fp_filepaths);
    fwrite(&newline, sizeof(char), 1, parser->fp_filepaths);

  }
}

void memory_processor(Parser *parser) {
  if (parser->type == 1) {

    // If we have a jpg, create a new frame
    if (strcmp(parser->ext, "jpg") == 0) {
      parser->color_buffer = new vector<char>();
      parser->color_buffer->reserve(parser->file_length);

      parser->writing_color = true;

      // Create frame and pair objects
      Frame *frame = new Frame();
      frame->index = parser->frames.size();
      parser->frames.push_back(frame);
    }

    // Add to the existing frame
    else if (strcmp(parser->ext, "png") == 0) {
      parser->depth_buffer = new vector<char>();
      parser->depth_buffer->reserve(parser->file_length);
      parser->writing_depth = true;
    }

    // For now, ignore everything else
  }
}