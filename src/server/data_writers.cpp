#include "data_writers.h"

// Write files individually to disk
void disk_writer(Parser *parser, bool commit) {
  int data_length = std::min(parser->file_length - parser->file_index, parser->buffer_length - parser->buffer_index);

  if (commit) {
    fwrite(parser->buffer + parser->buffer_index, sizeof(char), data_length, parser->fp);
  }

  parser->buffer_index += data_length;
  parser->file_index += data_length;

  parser->written = parser->file_index == parser->file_length;

  if (parser->written) {
    if (parser->fp != NULL) {
      fclose(parser->fp);
      parser->fp = NULL;
    }
    parser->file_index = 0;
    parser->file_length = 0;
    if (commit) fprintf(stderr, "Wrote to file %s\n", parser->path);
  } else {
    memset(parser->buffer, 0, BUFFER_SIZE);
  }
}

// Write files as blob to disk (one big file)
void blob_writer(Parser *parser, bool commit) {

  int data_length = std::min(parser->file_length - parser->file_index, parser->buffer_length - parser->buffer_index);

  if (commit) {
    fwrite(parser->buffer + parser->buffer_index, sizeof(char), data_length, parser->fp);
  }

  parser->buffer_index += data_length;
  parser->file_index += data_length;

  parser->written = parser->file_index == parser->file_length;

  if (parser->written) {
    parser->file_index = 0;
    parser->file_length = 0;
    if (commit) fprintf(stderr, "Wrote to file %s\n", parser->path);
  } else {
    memset(parser->buffer, 0, BUFFER_SIZE);
  }
}

// Keep files in memeory
void memory_writer(Parser *parser, bool commit) {
  int data_length = std::min(parser->file_length - parser->file_index, parser->buffer_length - parser->buffer_index);
  char *start = parser->buffer + parser->buffer_index;
  char *end = start + data_length;

  if (commit && parser->writing_color) {
    parser->color_buffer->insert(parser->color_buffer->begin() + parser->file_index, start, end);
  } else if (commit && parser->writing_depth) {
    parser->depth_buffer->insert(parser->depth_buffer->begin() + parser->file_index, start, end);
  }

  parser->buffer_index += data_length;
  parser->file_index += data_length;

  parser->written = parser->file_index == parser->file_length;

  if (parser->written) {

    parser->file_index = 0;
    parser->file_length = 0;

    if (commit && parser->writing_color) {
      parser->writing_color = false;
    }

    // If we've finished writing a depth frame, add the image pair to the
    // current frame before deleting the color / depth buffers
    else if (commit && parser->writing_depth) {
      fprintf(stderr, "******* Wrote to memory %lu %s\n", parser->frames.size() - 1, parser->path);
      
      parser->frames.back()->addImagePairFromBuffer(parser->color_buffer, parser->depth_buffer);

      // Call computeRigidTransform from second to last frame to get relative R_t
      if (parser->frames.size() > 1) {
        parser->frames.end()[-2]->computeRelativeTransform(parser->frames.back());
        parser->frames.back()->computeAbsoluteTransform(parser->frames.end()[-2]);
        parser->frames.back()->transformPointCloudCameraToWorld();

        if (parser->frames.size() % 8 == 0) {
          parser->frames.end()[-2]->writePointCloud(); // TODO: Write last point cloud!
        }
      } else if (parser->frames.size() == 1) {
        // First point cloud's world coordinates = camera coordinates
        parser->frames[0]->pairs[0]->pointCloud_world = parser->frames[0]->pairs[0]->pointCloud_camera;
        parser->frames[0]->writePointCloud();
      }

      delete parser->color_buffer;
      delete parser->depth_buffer;

      parser->writing_depth = false;
    }

    free(parser->path);
  } else {
    memset(parser->buffer, 0, BUFFER_SIZE);
  }
}