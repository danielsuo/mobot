#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <fstream>
#include <limits>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "cuSIFT/extras/rigidTransform.h"

#include "Pair.h"
#include "PointCloud.h"
#include "utilities.h"

using namespace std;

class Frame {
public:
  int index;
  int numDevices;
  vector<Pair *> pairs;
  vector<SiftMatch *> matches;
  float *Rt_relative;
  float *Rt_absolute;

  // Point cloud in camera coordinates
  PointCloud *pointCloud_camera;

  // Point cloud in world coordinates
  PointCloud *pointCloud_world;

  static int currIndex;

  /* Frame(); */
  Frame(int numDevices);
  ~Frame();

  bool isEmpty();
  bool isFull();
  void computeRelativeTransform(Frame *next);
  void computeRelativeTransform(Frame *next, float *Rt);
  void computeAbsoluteTransform(Frame *prev);

  void initializeFullFrame();
  void buildPointCloud(int pairIndex, float scaleRelativeToFirstCamera, float *extrinsicMatrixRelativeToFirstCamera);
  void transformPointCloudCameraToWorld();
  void writePointCloud();
  void writeIndices();
  void writeTimestamps();
};

#endif
