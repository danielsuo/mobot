#ifndef FRAME_H
#define FRAME_H

#include "lib/cuSIFT/extras/rigidTransform.h"

#include "Device.h"
#include "Pair.h"

#define THRESHOLD 50

using namespace std;

class Frame {
 public:
  int index;
  int numDevices;
  vector<Pair *> pairs;
  float *Rt_relative;
  float *Rt_absolute;

  static int currIndex;

  /* Frame(); */
  Frame(int numDevices);
  ~Frame();

  bool isFull();
  void pollDevices(vector<Device *> &devices);
  void computeRelativeTransform(Frame *next);
  void computeAbsoluteTransform(Frame *prev);
  void transformPointCloudCameraToWorld();
  void writePointCloud();
  void convert(int type);
};

#endif
