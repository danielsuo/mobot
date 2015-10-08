#ifndef PAIR_H
#define PAIR_H

#include "RGBD_utils.h"

using namespace std;

class Pair {
 public:
  cv::Mat color;
  cv::Mat depth;
  cv::Mat pointCloud;
  SiftData siftData;

  Pair(vector<char> *color_buffer, vector<char> *depth_buffer, Camera camera);
  Pair(string color_path, string depth_path, Camera camera);
  ~Pair();
  void transformPointCloud(float T[12]);
  int getMatched3DPoints(Pair *other, cv::Mat &lmatch, cv::Mat &rmatch);
  void convert(int type);

 private:
  void initPair(Camera camera);
  void processDepth();
  void createPointCloud(Camera camera);
  void computeSift();
};

#endif
