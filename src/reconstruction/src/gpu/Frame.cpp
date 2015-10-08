#include "Frame.h"

Frame::Frame() {}

Frame::~Frame() {
  fprintf(stderr, "Destroying frame\n");
  for (int i = 0; i < pairs.size(); i++) {
    delete pairs[i];
  }
}

void Frame::addImagePairFromFile(string color_path, string depth_path, Camera camera) {
  Pair *pair = new Pair(color_path, depth_path, camera);
  pairs.push_back(pair);
}

void Frame::computeRigidTransform(Frame *other, float T[12], float rigidtrans[12]) {

  Pair *lpair = pairs[0];
  Pair *rpair = other->pairs[0];

  lpair->transformPointCloud(T);

  // Container for sift match points
  cv::Mat lmatch(0, 3, cv::DataType<float>::type);
  cv::Mat rmatch(0, 3, cv::DataType<float>::type);

  int numMatchedPoints = lpair->getMatched3DPoints(rpair, lmatch, rmatch);
  cout << "Number matched points:" << numMatchedPoints << endl;

  int numMatches[1];
  int numLoops = 1000;
  numLoops = ceil(numLoops / 128) * 128;

  ransacfitRt(lmatch, rmatch, rigidtrans, numMatches, numLoops, 0.1);

  rpair->transformPointCloud(T);

  lmatch.release();
  rmatch.release();
}

void Frame::convert(int type) {
  for (int i = 0; i < pairs.size(); i++) {
    pairs[i]->convert(type);
  }
}
