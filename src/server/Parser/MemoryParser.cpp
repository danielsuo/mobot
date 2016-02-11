#include "Parser/MemoryParser.h"

MemoryParser::MemoryParser(int index, char *name) : Parser(index, name) {}

void MemoryParser::preprocess() {}

void MemoryParser::process() {
  if (type == 1) {

    // If we have a jpg, create a new frame
    if (strcmp(ext, "jpg") == 0) {
      color_buffer = new vector<char>();
      color_buffer->reserve(file_length);

      writing_color = true;
    }

    // Add to the existing frame
    else if (strcmp(ext, "png") == 0) {
      depth_buffer = new vector<char>();
      depth_buffer->reserve(file_length);
      writing_depth = true;
    }

    // For now, ignore everything else
  }
}

void MemoryParser::write(int data_length) {
  char *start = buffer + buffer_index;
  char *end = start + data_length;

  if (writing_color) {
    color_buffer->insert(color_buffer->begin() + file_index, start, end);
  } else if (writing_depth) {
    depth_buffer->insert(depth_buffer->begin() + file_index, start, end);
  }
}

void MemoryParser::postprocess() {
  if (writing_color) {
    writing_color = false;
  }

    // If we've finished writing a depth frame, add the image pair to the
    // current frame before deleting the color / depth buffers
  else if (writing_depth) {
    fprintf(stderr, "******* Wrote to memory %s\n", path);

    Pair *pair = new Pair(color_buffer, depth_buffer);
    pair->timestamp = timestamp;
    if (!queue->try_enqueue(pair)) {
      cerr << "Couldn't enqueue data!!" << endl;
      exit(-1);
    } else {
      queue_length++;
      // cerr << "Enqueing " << queue_length << "th pair to device " << index << endl;
    }

    delete color_buffer;
    delete depth_buffer;

    writing_depth = false;
  }
}