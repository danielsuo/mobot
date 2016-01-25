#include "Strategy.h"
#include "BundleAdjustAllStrategy.h"
#include "ReplicateMatlabStrategy.h"

Strategy *Strategy::createStrategy(StrategyType type) {
  switch(type) {
    case DefaultStrategyType:
    return new Strategy();
    break;

    case BundleAdjustAllStrategyType:
    return new BundleAdjustAllStrategy();
    break;

    case ReplicateMatlabStrategyType:
    return new ReplicateMatlabStrategy();
    break;
  }
}

Strategy::Strategy() {
  // grid = new Grid(100, 100);
}

Strategy::~Strategy() {
  // delete grid;
  for (int i = 0; i < frames.size(); i++) {
    delete frames[i];
  }
}

void Strategy::addFrame(Frame *frame) {
  frames.push_back(frame);
}

void Strategy::processMostRecentFrame(vector<Device *> &devices) {
      // Construct point cloud from individual pairs.
  for (int i = 0; i < devices.size(); i++) {
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
    if (frames.size() % 8 == 0) {
      frames.end()[-2]->writePointCloud();
    }
  }

  // If we have a single frame, set the initial point cloud in world
  // coordinates to camera coordinates (i.e., identity extrinsic matrix)
  else if (frames.size() == 1) {
    frames[0]->pointCloud_world = frames[0]->pointCloud_camera;
  }
}

void Strategy::processLastFrame() {}