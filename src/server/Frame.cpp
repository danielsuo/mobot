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
}

bool Frame::isFull() {
  bool full = true;
  for (int i = 0; i < numDevices; i++) {
    full &= pairs[i] != NULL;
  }

  return full;
}

void Frame::pollDevices(vector<Device *> &devices) {
  for (int i = 0; i < numDevices; i++) {
    Pair *pair;

    // Try to dequeue pair if we have room in the frame (i.e., not NULL)
    if (pairs[i] == NULL && devices[i]->queue.try_dequeue(pair)) {

      // Check against all existing pairs have timestamps within THRESHOLD of
      // this new pair
      for (int j = 0; j < numDevices; j++) {

        // Ignore pairs in the same slot or if there isn't any pair data
        if (i != j && pairs[j] != NULL) {

          // If the timestamp is out of range, boot the offending pair
          if (pair->timestamp - pairs[j]->timestamp > THRESHOLD) {
            cerr << "Booting pairs with timestamp difference of " << pair->timestamp - pairs[j]->timestamp << endl;
            delete pairs[j];
            pairs[j] = NULL;
          }
        }
      }

      // Add the new pair
      pairs[i] = pair;
    }

    // If data already exists or we don't have new data, continue
  }
}

void Frame::computeRelativeTransform(Frame *next) {
  fprintf(stderr, "Computing relative transform for frame: %d\n", index);
  Pair *curr_pair = pairs[0];
  Pair *next_pair = next->pairs[0];

  // Container for sift match points
  cv::Mat curr_match(0, 3, cv::DataType<float>::type);
  cv::Mat next_match(0, 3, cv::DataType<float>::type);

  int numMatchedPoints = curr_pair->getMatched3DPoints(next_pair, curr_match, next_match);
  fprintf(stderr, "Number matched points: %d\n", numMatchedPoints);

  // std::ostringstream matchPath;
  // matchPath << "../result/match/match";
  // matchPath << index + 1;
  // ReadMATLABMatchData(curr_match, next_match, matchPath.str().c_str());
  
  int numMatches[1];
  int numLoops = 1024;
  numLoops = ceil(numLoops / 128) * 128;

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
  Pair *prev_pair = prev->pairs[0];
  Pair *curr_pair = pairs[0];

  // Absolute transform from previous frame's pose to world coordinates
  float *A = prev->Rt_absolute;

  // Relative transform from current frame's pose to previous frame's pose
  float *R = prev->Rt_relative;

  // Absolute transform of current frame's pose to world coordinates
  float *M = Rt_absolute;

  M[ 0] = A[0] * R[0] + A[1] * R[4] + A[2] * R[8];
  M[ 1] = A[0] * R[1] + A[1] * R[5] + A[2] * R[9];
  M[ 2] = A[0] * R[2] + A[1] * R[6] + A[2] * R[10];
  M[ 3] = A[0] * R[3] + A[1] * R[7] + A[2] * R[11] + A[3];
  M[ 4] = A[4] * R[0] + A[5] * R[4] + A[6] * R[8];
  M[ 5] = A[4] * R[1] + A[5] * R[5] + A[6] * R[9];
  M[ 6] = A[4] * R[2] + A[5] * R[6] + A[6] * R[10];
  M[ 7] = A[4] * R[3] + A[5] * R[7] + A[6] * R[11] + A[7];
  M[ 8] = A[8] * R[0] + A[9] * R[4] + A[10] * R[8];
  M[ 9] = A[8] * R[1] + A[9] * R[5] + A[10] * R[9];
  M[10] = A[8] * R[2] + A[9] * R[6] + A[10] * R[10];
  M[11] = A[8] * R[3] + A[9] * R[7] + A[10] * R[11] + A[11];

  // for (int i = 0; i < 12; i++) {
  //   fprintf(stderr, "%0.10f ", A[i]);
  //   if ((i + 1) % 4 == 0) fprintf(stderr, "\n");
  // }
  // for (int i = 0; i < 12; i++) {
  //   fprintf(stderr, "%0.10f ", R[i]);
  //   if ((i + 1) % 4 == 0) fprintf(stderr, "\n");
  // }
  // for (int i = 0; i < 12; i++) {
  //   fprintf(stderr, "%0.10f ", Rt_absolute[i]);
  //   if ((i + 1) % 4 == 0) fprintf(stderr, "\n");
  // }
}

void Frame::transformPointCloudCameraToWorld() {
  pairs[0]->pointCloud_world = pairs[0]->transformPointCloud(pairs[0]->pointCloud_camera, Rt_absolute);
}

void Frame::writePointCloud() {
  std::ostringstream ply_path;
  ply_path << "../result/pc_";
  ply_path << index;
  ply_path << ".ply";
  pairs[0]->writePLY(ply_path.str().c_str());
}

void Frame::convert(int type) {
  for (int i = 0; i < pairs.size(); i++) {
    pairs[i]->convert(type);
  }
}
