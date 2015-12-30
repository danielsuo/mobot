#ifndef FRAME_H
#define FRAME_H

#include <fstream>
#include <limits>
#include "lib/cuSIFT/extras/rigidTransform.h"

#include "Pair.h"
#include "PointCloud.h"
#include "utilities.h"

#define THRESHOLD 50

using namespace std;

class Frame {
public:
  int index;
  int numDevices;
  vector<Pair *> pairs;
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
  void computeAbsoluteTransform(Frame *prev);

  void initializeFullFrame();
  void buildPointCloud(int pairIndex, float scaleRelativeToFirstCamera, float *extrinsicMatrixRelativeToFirstCamera);
  void transformPointCloudCameraToWorld();
  void writePointCloud();
  void writeIndices();
  void writeTimestamps();
};

#endif
