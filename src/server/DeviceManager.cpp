#include "DeviceManager.h"

DeviceManager::DeviceManager(DeviceOutputMode mode) {
  // Parameters *parameters = new Parameters("../", "data/");
  this->mode = mode;
  failedPolls = 0;
  numDevices = 0;
}

DeviceManager::~DeviceManager() {
  for (int i = 0; i < numDevices; i++) {
    delete devices[i];
  }

  for (int i = 0; i < frames.size(); i++) {
    delete frames[i];
  }
}

void blob_preprocessor(Parser *parser);
void disk_preprocessor(Parser *parser);
void memory_preprocessor(Parser *parser);

void blob_processor(Parser *parser);
void disk_processor(Parser *parser);
void memory_processor(Parser *parser);

void blob_writer(Parser *parser, bool commit);
void disk_writer(Parser *parser, bool commit);
void memory_writer(Parser *parser, bool commit);

void DeviceManager::initDevice(Device *device) {
  device->manager = this;
  devices.push_back(device);
  numDevices++;

  // Functions to define how to output data. Defined below.
  switch (mode) {
    case DeviceOutputModeBlob:
    device->parser->preprocessor = blob_preprocessor;
    device->parser->processor = blob_processor;
    device->parser->writer = blob_writer;
    break;

    case DeviceOutputModeDisk:
    device->parser->preprocessor = disk_preprocessor;
    device->parser->processor = disk_processor;
    device->parser->writer = disk_writer;
    break;

    case DeviceOutputModeMemory:
    device->parser->preprocessor = memory_preprocessor;
    device->parser->processor = memory_processor;
    device->parser->writer = memory_writer;
    break;
  }
}

void DeviceManager::runLoop() {
  Frame *currFrame = new Frame(numDevices);

  while (1) {
    currFrame->pollDevices(devices);

    if (currFrame->isFull()) {

      frames.push_back(currFrame);
      currFrame = new Frame(numDevices);

      // Construct point cloud from individual pairs
      frames.back()->buildPointCloud(devices);

      // Ideally we could use a thread, but must use single thread because
      // each subsequent frame depends on earlier frames. If we have more than
      // one frame, compute relative transforms
      if (frames.size() > 1) {

        // Compute relative transform between previous frame and current frame
        frames.end()[-2]->computeRelativeTransform(frames.back());

        // Compute absolute coordinates of our current frame using absolute
        // coordinates of previous frame and the relative transform between
        // the two
        frames.back()->computeAbsoluteTransform(frames.end()[-2]);

        // Finally, transform the point cloud to world coordinates
        frames.back()->transformPointCloudCameraToWorld();

        // For debugging, write every nth point cloud
        // if (frames.size() % 8 == 0) {
          frames.end()[-2]->writePointCloud();
        // }
      }

      // If we have a single frame, set the initial point cloud in world
      // coordinates to camera coordinates (i.e., identity extrinsic matrix)
      else if (frames.size() == 1) {
        frames[0]->pointCloud_world = frames[0]->pointCloud_camera;
      }

      // Reset number of failed polls
      failedPolls = 0;
    } else {
      if (currFrame->isEmpty()) failedPolls++;

      if (failedPolls > MAX_NUM_FAILED_POLLS) {
        cerr << "Haven't seen any data in a while. Good bye!" << endl;
        exit(0);
      }
    }

    usleep(5000);
  }
}

void DeviceManager::addDeviceByFileDescriptor(char *name, int fd) {
  Device *device = new Device(name, fd);

  initDevice(device);
}

void DeviceManager::addDeviceByFilePath(char *name, char *path) {
  Device *device = new Device(name, path);

  initDevice(device);
}

void DeviceManager::addDeviceByStringIPAddress(char *name, char *address, int port) {
  Device *device = new Device(name, address, port);

  initDevice(device);
}

void DeviceManager::addDeviceByIPAddress(uint32_t addr, uint16_t port) {
  Device *device = new Device(addr, port);

  initDevice(device);
}

Device *DeviceManager::getDeviceByIPAddress(uint32_t addr, uint16_t port) {
  struct sockaddr_in saddr;

  saddr.sin_addr.s_addr = addr;
  printf("Looking for device with address %s:%u\n", inet_ntoa(saddr.sin_addr), port);

  for (int i = 0; i < numDevices; i++) {
    saddr.sin_addr.s_addr = devices[i]->addr;
    printf("Found device with address %s:%u\n", inet_ntoa(saddr.sin_addr), devices[i]->dat_port);

    // Authenticate with address only, not port
    if (addr == devices[i]->addr) {
      printf("Found existing device!\n");

      // Update cmd_port
      devices[i]->dat_port = port;
      return devices[i];
    }
  }

  printf("Creating new device!\n");

  // We aren't expecting connections that aren't pre-defined
  addDeviceByIPAddress(addr, port);
  return devices[numDevices - 1];
}

void DeviceManager::digest() {
  for (int i = 0; i < numDevices; i++) {
    cerr << "Digesting device " << i << endl;
    Pair::currIndex = 0;
    devices[i]->digest();
  }
}

/******************************************************************************
 *
 * Device output functions
 *
 ******************************************************************************/

// DEVICE PREPROCESSORS

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

// DEVICE PROCESSORS

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
  // fprintf(stderr, "Processing %s\n", parser->device->name);
  fwrite(parser->buffer + parser->metadata_index, sizeof(char), parser->metadata_length, parser->fp);

  // Only write timestamp if it's color or depth image
  if (strcmp(parser->ext, "jpg") == 0 || strcmp(parser->ext, "png") == 0) {
    char newline = '\n';

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

// DEVICe WRITERS

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
      fprintf(stderr, "******* Wrote to memory %s\n", parser->path);

      Pair *pair = new Pair(parser->color_buffer, parser->depth_buffer);
      pair->timestamp = parser->timestamp;
      if (!parser->device->queue->try_enqueue(pair)) {
        cerr << "Couldn't enqueue data!!" << endl;
        exit(-1);
      } else {
        parser->device->queue_length++;
        cerr << "Enqueing " << parser->device->queue_length << "th pair to device " << parser->device->index << endl;
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
