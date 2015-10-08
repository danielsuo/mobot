#ifndef FRAME_H
#define FRAME_H

#include "RGBD_utils.h"
#include "Pair.h"

using namespace std;

class Frame {
 public:
  vector<Pair> pairs;

  Frame();

  void addImagePairFromFile(string color_path, string depth_path, Camera camera);
  void computeRigidTransform(Frame &other, float T[12], float rigidtrans[12]);
  void convert(int type);
};

#endif
