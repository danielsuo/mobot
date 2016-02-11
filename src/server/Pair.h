#ifndef PAIR_H
#define PAIR_H

#include <cmath>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "extras/debug.h"
#include "extras/matching.h"
#include "extras/rigidTransform.h"

#include "PointCloud.h"

using namespace std;

class Pair {
 public:
  int width;
  int height;

  SiftData siftData;
  PointCloud *pointCloud;

  static int currIndex;

  int pair_index;
  int frame_index;
  double timestamp;

  Pair(vector<char> *color_buffer, vector<char> *depth_buffer);
  Pair(string color_path, string depth_path);
  ~Pair();

  void deletePointCloud();
  void computeSift(cv::Mat gray);
  void computeSift3D();

 private:
  void initPair(cv::Mat gray);
};

#endif
