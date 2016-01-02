#include "DeviceManager.h"

DeviceManager::DeviceManager() {
  numFailedPolls = 0;
}

DeviceManager::~DeviceManager() {
  for (int i = 0; i < devices.size(); i++) {
    delete devices[i];
  }

  for (int i = 0; i < frames.size(); i++) {
    delete frames[i];
  }
}

void DeviceManager::runLoop() {
  Frame *currFrame = new Frame(devices.size());

  while (1) {
    pollDevices(currFrame);

    if (currFrame->isFull()) {

      frames.push_back(currFrame);
      currFrame = new Frame(devices.size());

      // Construct point cloud from individual pairs. We don't need to scale /
      // transform first camera
      for (int i = 1; i < devices.size(); i++) {
        frames.back()->buildPointCloud(i, devices[i]->scaleRelativeToFirstCamera, devices[i]->extrinsicMatrixRelativeToFirstCamera);
      }

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
      numFailedPolls = 0;
    } else {
      if (currFrame->isEmpty()) numFailedPolls++;

      if (numFailedPolls > MAX_NUM_FAILED_POLLS) {
        cerr << "Haven't seen any data in a while. Good bye!" << endl;
        exit(0);
      }

      usleep(5000);
    }
  }
}

void DeviceManager::pollDevices(Frame *frame) {
  cerr << endl;
  cerr << "Polling " << devices.size() << " devices" << endl;
  for (int i = 0; i < devices.size(); i++) {
    Pair *pair = nullptr;

    cerr << "Device " << i << " has " << devices[i]->parser->queue_length << " pairs" << endl;

    bool addPair = false;

    // Try to dequeue pair if we have room in the frame (i.e., not nullptr)
    while (!addPair && frame->pairs[i] == nullptr && devices[i]->parser->queue->try_dequeue(pair)) {
      cerr << "Pair dequeued" << endl;
      devices[i]->parser->queue_length--;

      // Check against all existing pairs have timestamps within THRESHOLD of
      // this new pair
      for (int j = 0; j < devices.size(); j++) {

        // Ignore pairs in the same slot or if there isn't any pair data
        if (i != j && frame->pairs[j] != nullptr) {

          // If the timestamp is out of range, boot the offending pair
          if (abs(pair->timestamp - frame->pairs[j]->timestamp) > MAX_USEC_FRAME_WINDOW) {
            // exit(0);

            // If pair is more recent than frame->pairs[j], delete frame->pairs[j]
            if (pair->timestamp > frame->pairs[j]->timestamp) {
              cerr << "Booting EXISTING pair from device " << j << endl;
              delete frame->pairs[j];
              frame->pairs[j] = nullptr;
              addPair = true;
            }

            // Otherwise, continue and ignore the pair we've just read
            else {
              cerr << "Booting NEW pair from device " << i << endl;
              delete pair;
              pair = nullptr;
              addPair = false;
              break;
            }
          }
        }
      }

      // Add the new pair
      frame->pairs[i] = pair;

      cerr << "Adding new pair" << endl;
    } 

    // else if (frame->pairs[i] == nullptr) {
    //   cerr << "Couldn't dequeue anything!" << endl;
    // }

    // If data already exists or we don't have new data, continue
  }
}


void DeviceManager::addDeviceByFilePath(char *name, char *path, ParserOutputMode mode) {
  Device *device = new Device(name, path, mode);

  devices.push_back(device);
}

void DeviceManager::addDeviceByStringIPAddress(char *name, char *address, int port, ParserOutputMode mode) {
  Device *device = new Device(name, address, port, mode);

  devices.push_back(device);
}

void DeviceManager::addDeviceByIPAddress(uint32_t addr, uint16_t port, ParserOutputMode mode) {
  Device *device = new Device(addr, port, mode);

  devices.push_back(device);
}

Device *DeviceManager::getDeviceByIPAddress(uint32_t addr, uint16_t port) {
  struct sockaddr_in saddr;

  saddr.sin_addr.s_addr = addr;
  printf("Looking for device with address %s:%u\n", inet_ntoa(saddr.sin_addr), port);

  for (int i = 0; i < devices.size(); i++) {
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

  // We aren't expecting connections that aren't pre-defined so ignore for now
  // addDeviceByIPAddress(addr, port);
  // return devices[devices.size() - 1];
}

void DeviceManager::digest() {
  for (int i = 0; i < devices.size(); i++) {
    cerr << "Digesting device " << i << endl;
    Pair::currIndex = 0;
    devices[i]->digest();
  }
}