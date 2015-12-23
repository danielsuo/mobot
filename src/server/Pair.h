#ifndef PAIR_H
#define PAIR_H

#include <GL/osmesa.h>
#include <GL/glu.h>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "lib/cuSIFT/extras/matching.h"
#include "lib/cuSIFT/extras/rigidTransform.h"

using namespace std;

struct Camera {
  float cx, cy, fx, fy;
  Camera(float cx, float cy, float fx, float fy) : cx(cx), cy(cy), fx(fx), fy(fy) {};
};

class Pair {
 public:
  cv::Mat color;
  cv::Mat gray;
  cv::Mat depth;

  // Point cloud in camera coordinates
  cv::Mat pointCloud_camera; // TODO: move to Frame

  // Point cloud in world coordinates
  cv::Mat pointCloud_world;
  SiftData siftData;

  static Camera *camera;

  int frame_index;

  Pair(vector<char> *color_buffer, vector<char> *depth_buffer, int index);
  Pair(string color_path, string depth_path, int index);
  ~Pair();
  cv::Mat transformPointCloud(cv::Mat pointCloud, float T[12]);
  cv::Mat createPointCloud(Camera *camera);
  int getMatched3DPoints(Pair *other, cv::Mat &lmatch, cv::Mat &rmatch);
  void convert(int type);
  void writePLY(const char *plyfile);
  void readSIFT(const char *siftfile);

 private:
  void initPair(int index);
  void bitShiftDepth();
  void linearizeDepth();
  void projectPointCloud(Camera *camera);
  void computeSift();
};

#endif
