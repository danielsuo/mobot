#ifndef PAIR_H
#define PAIR_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "lib/cuSIFT/extras/matching.h"
#include "lib/cuSIFT/extras/rigidTransform.h"

#include "PointCloud.h"

using namespace std;

class Pair {
 public:
  cv::Mat gray;

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
  int getMatched3DPoints(Pair *other, cv::Mat &lmatch, cv::Mat &rmatch);
  void convert(int type);

 private:
  void initPair();
  void computeSift();
};

#endif
