#include "Frame.h"

// Frame::Frame() {}

Frame::Frame(Parameters *parameters) {
  this->parameters = parameters;
  Rt_relative = new float[12]();
  Rt_absolute = new float[12]();

  // Initialize Rt_relative to identity transform
  Rt_relative[0] = Rt_relative[5] = Rt_relative[10] = 1;
  Rt_absolute[0] = Rt_absolute[5] = Rt_absolute[10] = 1;
}

Frame::~Frame() {
  for (int i = 0; i < pairs.size(); i++) {
    delete pairs[i];
  }
  delete Rt_relative;
  delete Rt_absolute;
}

void Frame::addImagePairFromBuffer(vector<char> *color_buffer, vector<char> *depth_buffer) {
  Pair *pair = new Pair(color_buffer, depth_buffer, parameters);
  pairs.push_back(pair);
}

void Frame::addImagePairFromFile(string color_path, string depth_path) {
  Pair *pair = new Pair(color_path, depth_path, parameters);
  pairs.push_back(pair);
}

void Frame::computeRigidTransform(Frame *other) {

  Pair *lpair = pairs[0];
  Pair *rpair = other->pairs[0];

  // lpair->transformPointCloud(T);

  // Container for sift match points
  cv::Mat lmatch(0, 3, cv::DataType<float>::type);
  cv::Mat rmatch(0, 3, cv::DataType<float>::type);

  int numMatchedPoints = lpair->getMatched3DPoints(rpair, lmatch, rmatch);
  cout << "Number matched points:" << numMatchedPoints << endl;

  int numMatches[1];
  int numLoops = 1000;
  numLoops = ceil(numLoops / 128) * 128;

  ransacfitRt(lmatch, rmatch, Rt_relative, numMatches, numLoops, 0.1);

  // rpair->transformPointCloud(Rt_relative);

  cv::Mat imRresult = PrintMatchData(lpair->siftData, rpair->siftData, lpair->color, rpair->color);
  printf("write image\n");
  std::ostringstream imresult_path;
  imresult_path << "../result/imRresult_beforeransac_";
  imresult_path << index;
  imresult_path << ".jpg";
  cv::imwrite(imresult_path.str(), imRresult);
  imRresult.release();

  lmatch.release();
  rmatch.release();
}

void Frame::convert(int type) {
  for (int i = 0; i < pairs.size(); i++) {
    pairs[i]->convert(type);
  }
}
