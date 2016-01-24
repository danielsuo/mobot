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
    Pair *pair = nullptr;
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
    empty &= pairs[i] == nullptr;
  }
}

bool Frame::isFull() {
  bool full = true;
  for (int i = 0; i < numDevices; i++) {
    full &= pairs[i] != nullptr;
  }

  // if (full) {
  //   writeIndices();
  //   writeTimestamps();
  // }

  return full;
}

// TODO: Not particularly memory-efficient. Can preallocate rather than keep
// two copies
void Frame::buildPointCloud(int pairIndex, float scaleRelativeToFirstCamera, float *extrinsicMatrixRelativeToFirstCamera) {
  // pairs[pairIndex]->pointCloud->scalePointCloud(scaleRelativeToFirstCamera);
  // pairs[pairIndex]->pointCloud->transformPointCloud(extrinsicMatrixRelativeToFirstCamera);
  pairs[pairIndex]->computeSift3D();
  pointCloud_camera->append(pairs[pairIndex]->pointCloud);
}

void Frame::computeRelativeTransform(Frame *next) {
  computeRelativeTransform(next, Rt_relative);
}

vector<SiftMatch *> Frame::computeRelativeTransform(Frame *next, float *Rt) {
  fprintf(stderr, "Computing relative transform for frame: %d\n", index);

  for (int i = 0; i < numDevices; i++) {
    Pair *curr_pair = pairs[i];
    Pair *next_pair = next->pairs[i];

    vector<SiftMatch *> currMatches = MatchSiftData(curr_pair->siftData, next_pair->siftData, MatchSiftDistanceL2, 100.0f, 0.36, MatchType3D);
    matches.insert(matches.end(), currMatches.begin(), currMatches.end());

    fprintf(stderr, "\tNumber filtered matched points: %lu\n", currMatches.size());

    // We won't delete the point cloud of the last frame until that pair is
    // deleted. Oh well.
    // curr_pair->deletePointCloud();
  }

  // string path = "../result/match/match" + to_string(index + 1) + "_" + to_string(next->index + 1);
  // cerr << "Getting matches from (" << index << ", " << next->index << ")" << endl;
  // matches = ReadMATLABMatchData(path.c_str());

  int numMatches[1];
  int numLoops = 1024;
  // numLoops = ceil(numLoops / 128) * 128;

  // if (matches.size() < 3) {
  //   cerr << matches.size() << endl;
  //   exit(-1);
  // }

  EstimateRigidTransform(matches, Rt, numMatches, numLoops, 0.05, RigidTransformType3D);

  fprintf(stderr, "frame %d + %d: # ransac inliers = %d/%lu = %0.4f%%\n", index, next->index, numMatches[0], matches.size(), (float)numMatches[0] / matches.size() * 100);

  // string path = "../result/Rt/Rt" + to_string(index + 1);
  // ReadMATLABRt(Rt, path.c_str());

  // cv::Mat imRresult = PrintMatchData(curr_pair->siftData, next_pair->siftData, curr_pair->gray, next_pair->gray);
  // printf("write image\n");

  // std::ostringstream imresult_path;
  // imresult_path << "../result/imRresult_beforeransac_";
  // imresult_path << index;
  // imresult_path << ".jpg";
  // cv::imwrite(imresult_path.str().c_str(), imRresult);
  // imRresult.release();

  return matches;
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
  ply_path << "../result/ply/pc_";
  ply_path << index;
  ply_path << ".ply";
  pointCloud_world->writePLY(ply_path.str().c_str());
}

void Frame::writeIndices() {
  ofstream file;
  file.open("../result/indices.txt", ios::app);

  file << index << " ";

  for (int i = 0; i < numDevices; i++) {
    file << pairs[i]->pair_index << " ";
  }

  file << endl;

  file.close();
}

void Frame::writeTimestamps() {
  ofstream file;
  file.open("../result/timestamps.txt", ios::app);

  file << index << " ";

  double minTimestamp = DBL_MAX;

  for (int i = 0; i < numDevices; i++) {
    minTimestamp = min(minTimestamp, pairs[i]->timestamp);
    fprintf(stderr, "Min timestamp: %0.9f\n", minTimestamp);
  }

  for (int i = 0; i < numDevices; i++) {
    file << pairs[i]->timestamp - minTimestamp << " ";
    fprintf(stderr, "Timestamp for pair %d: %0.9f\n", i, pairs[i]->timestamp - minTimestamp);
  }

  file << endl;

  file.close();
}