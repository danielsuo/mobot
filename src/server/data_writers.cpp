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
void memory_writer(Data *data) {
  int data_length = std::min(data->file_length - data->file_index, data->buffer_length - data->buffer_index);
  char *start = data->buffer + data->buffer_index;
  char *end = start + data_length;

  if (data->writing_color) {
    data->color_buffer->insert(data->color_buffer->begin() + data->file_index, start, end);
  } else if (data->writing_depth) {
    data->depth_buffer->insert(data->depth_buffer->begin() + data->file_index, start, end);
  }

  data->buffer_index += data_length;
  data->file_index += data_length;

  data->written = data->file_index == data->file_length;

  if (data->written) {

    data->file_index = 0;
    data->file_length = 0;

    if (data->writing_color) {
      data->writing_color = false;
    }

    // If we've finished writing a depth frame, add the image pair to the
    // current frame before deleting the color / depth buffers
    else if (data->writing_depth) {
      fprintf(stderr, "******* Wrote to memory %lu %s\n", data->frames.size() - 1, data->path);
      
      data->frames.back()->addImagePairFromBuffer(data->color_buffer, data->depth_buffer);

      // Call computeRigidTransform from second to last frame to get relative R_t
      if (data->frames.size() > 1) {
        data->frames.end()[-2]->computeRelativeTransform(data->frames.back());
        data->frames.back()->computeAbsoluteTransform(data->frames.end()[-2]);
        data->frames.back()->transformPointCloudCameraToWorld();

        if (data->frames.size() % 10 == 0) {
          data->frames.end()[-2]->writePointCloud(); // TODO: Write last point cloud!
        }
      } else if (data->frames.size() == 1) {
        // First point cloud's world coordinates = camera coordinates
        data->frames[0]->pairs[0]->pointCloud_world = data->frames[0]->pairs[0]->pointCloud_camera;
          data->frames[0]->writePointCloud();
      }

      delete data->color_buffer;
      delete data->depth_buffer;

      data->writing_depth = false;
    }

    free(data->path);
  } else {
    memset(data->buffer, 0, BUFFER_SIZE);
  }
}