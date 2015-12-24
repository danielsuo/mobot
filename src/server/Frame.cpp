#include "Frame.h"

int Frame::currIndex = 0;

Frame::Frame(int numDevices) {
  index = currIndex++;

  Rt_relative = new float[12]();
  Rt_absolute = new float[12]();

  // Initialize Rt_relative to identity transform
  Rt_relative[0] = Rt_relative[5] = Rt_relative[10] = 1;
  Rt_absolute[0] = Rt_absolute[5] = Rt_absolute[10] = 1;

  this->numDevices = numDevices;

  pointCloud_camera = new PointCloud();
  pointCloud_world = new PointCloud();

  for (int i = 0; i < numDevices; i++) {
    Pair *pair = NULL;
    pairs.push_back(pair);
  }
}

Frame::~Frame() {
  for (int i = 0; i < pairs.size(); i++) {
    delete pairs[i];
  }
  delete Rt_relative;
  delete Rt_absolute;

  delete pointCloud_camera;
  delete pointCloud_world;
}

bool Frame::isEmpty() {
  bool empty = true;
  for (int i = 0; i < numDevices; i++) {
    empty &= pairs[i] == NULL;
  }
}

bool Frame::isFull() {
  bool full = true;
  for (int i = 0; i < numDevices; i++) {
    full &= pairs[i] != NULL;
  }

  return full;
}

void Frame::pollDevices(vector<Device *> &devices) {
  cerr << "Polling " << numDevices << " devices" << endl;
  for (int i = 0; i < numDevices; i++) {
    Pair *pair = NULL;

    cerr << "Device " << i << " has " << devices[i]->queue_length << " pairs" << endl;

    // Try to dequeue pair if we have room in the frame (i.e., not NULL)
    if (pairs[i] == NULL && devices[i]->queue->try_dequeue(pair)) {
      cerr << "Pair dequeued" << endl;

      // Check against all existing pairs have timestamps within THRESHOLD of
      // this new pair
      for (int j = 0; j < numDevices; j++) {

        // Ignore pairs in the same slot or if there isn't any pair data
        if (i != j && pairs[j] != NULL) {

          // If the timestamp is out of range, boot the offending pair
          if (pair->timestamp - pairs[j]->timestamp > THRESHOLD) {
            // exit(0);
            cerr << "Booting pairs with timestamp difference of " << pair->timestamp - pairs[j]->timestamp << endl;
            delete pairs[j];
            pairs[j] = NULL;
          }
        }
      }

      // Add the new pair
      pairs[i] = pair;
      devices[i]->queue_length--;

      cerr << "Adding new pair" << endl;
    } else if (pairs[i] == NULL) {
      cerr << "Couldn't dequeue anything!" << endl;
    }

    // If data already exists or we don't have new data, continue
  }
}

// TODO: Not particularly memory-efficient. Can preallocate rather than keep
// two copies
void Frame::buildPointCloud(vector<Device *> &devices) {
  for (int i = 0; i < numDevices; i++) {
    pairs[i]->pointCloud->scalePointCloud(devices[i]->scaleRelativeToFirstCamera);
    pairs[i]->pointCloud->transformPointCloud(devices[i]->extrinsicMatrixRelativeToFirstCamera);
  }

  pointCloud_camera->copy(pairs[0]->pointCloud);

  for (int i = 1; i < numDevices; i++) {
    pointCloud_camera->append(pairs[i]->pointCloud);
  }
}

void Frame::computeRelativeTransform(Frame *next) {
  fprintf(stderr, "Computing relative transform for frame: %d\n", index);

  // Container for sift match points
  cv::Mat curr_match(0, 3, cv::DataType<float>::type);
  cv::Mat next_match(0, 3, cv::DataType<float>::type);

  for (int i = 0; i < numDevices; i++) {
    Pair *curr_pair = pairs[i];
    Pair *next_pair = next->pairs[i];

    cv::Mat curr_match_tmp(0, 3, cv::DataType<float>::type);
    cv::Mat next_match_tmp(0, 3, cv::DataType<float>::type);

    int numMatchedPoints = curr_pair->getMatched3DPoints(next_pair, curr_match_tmp, next_match_tmp);
    fprintf(stderr, "\tNumber matched points: %d\n", numMatchedPoints);

    curr_match.push_back(curr_match_tmp);
    next_match.push_back(next_match_tmp);
    curr_match_tmp.release();
    next_match_tmp.release();

    // We won't delete the point cloud of the last frame until that pair is
    // deleted. Oh well.
    curr_pair->deletePointCloud();
  }

  // std::ostringstream matchPath;
  // matchPath << "../result/match/match";
  // matchPath << index + 1;
  // ReadMATLABMatchData(curr_match, next_match, matchPath.str().c_str());
  
  int numMatches[1];
  int numLoops = 1024;
  numLoops = ceil(numLoops / 128) * 128;

  if (curr_match.size().height < 3) {
    cerr << curr_match.size().height << endl;
    exit(-1);
  }

  EstimateRigidTransform(curr_match, next_match, Rt_relative, numMatches, numLoops, 0.05);

  // std::ostringstream RtPath;
  // RtPath << "../result/Rt/Rt";
  // RtPath << index + 1;
  // ReadMATLABRt(Rt_relative, RtPath.str().c_str());

  // cv::Mat imRresult = PrintMatchData(curr_pair->siftData, next_pair->siftData, curr_pair->gray, next_pair->gray);
  // printf("write image\n");

  // std::ostringstream imresult_path;
  // imresult_path << "../result/imRresult_beforeransac_";
  // imresult_path << index;
  // imresult_path << ".jpg";
  // cv::imwrite(imresult_path.str().c_str(), imRresult);
  // imRresult.release();

  curr_match.release();
  next_match.release();
}

void Frame::computeAbsoluteTransform(Frame *prev) {

  // Absolute transform from previous frame's pose to world coordinates
  float *A = prev->Rt_absolute;

  // Relative transform from current frame's pose to previous frame's pose
  float *R = prev->Rt_relative;

  // Absolute transform of current frame's pose to world coordinates
  float *M = Rt_absolute;

  // M = A * R
  composeTransform(A, R, M);
}

void Frame::transformPointCloudCameraToWorld() {
  pointCloud_world->copy(pointCloud_camera);
  pointCloud_world->transformPointCloud(Rt_absolute);
}

void Frame::writePointCloud() {
  std::ostringstream ply_path;
  ply_path << "../result/pc_";
  ply_path << index;
  ply_path << ".ply";
  pointCloud_world->writePLY(ply_path.str().c_str());
}