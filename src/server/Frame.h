#ifndef FRAME_H
#define FRAME_H

#include "lib/cuSIFT/RGBD_utils.h"
#include "Parameters.h"
#include "Pair.h"

using namespace std;

class Frame {
 public:
  int index;
  vector<Pair *> pairs;
  Parameters *parameters;
  float *Rt_relative;
  float *Rt_absolute;

  /* Frame(); */
  Frame(Parameters *parameters);
  ~Frame();

  void addImagePairFromBuffer(vector<char> *color_buffer, vector<char> *depth_buffer);
  void addImagePairFromFile(string color_path, string depth_path);
  void computeRelativeTransform(Frame *next);
  void computeAbsoluteTransform(Frame *prev);
  void transformPointCloudCameraToWorld();
  void writePointCloud();
  void convert(int type);
};

#endif