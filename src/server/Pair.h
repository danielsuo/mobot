#ifndef PAIR_H
#define PAIR_H

#include <GL/osmesa.h>
#include <GL/glu.h>
#include "lib/CudaSift/RGBD_utils.h"
#include "Parameters.h"

using namespace std;

class Pair {
 public:
  cv::Mat color;
  cv::Mat gray;
  cv::Mat depth;
  cv::Mat pointCloud; // TODO: move to Frame
  SiftData siftData;

  Pair(vector<char> *color_buffer, vector<char> *depth_buffer, Parameters *parameters);
  Pair(string color_path, string depth_path, Parameters *parameters);
  ~Pair();
  void transformPointCloud(float T[12]);
  int getMatched3DPoints(Pair *other, cv::Mat &lmatch, cv::Mat &rmatch);
  void convert(int type);

 private:
  void initPair(Parameters *parameters);
  void bitShiftDepth();
  void linearizeDepth();
  void createPointCloud(Camera *camera);
  void projectPointCloud(Camera *camera);
  void computeSift();
};

#endif
