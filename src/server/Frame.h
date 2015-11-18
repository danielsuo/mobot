#ifndef FRAME_H
#define FRAME_H

#include "lib/CudaSift/RGBD_utils.h"
#include "Parameters.h"
#include "Pair.h"

using namespace std;

class Frame {
 public:
  int index;
  vector<Pair *> pairs;
  Parameters *parameters;

  /* Frame(); */
  Frame(Parameters *parameters);
  ~Frame();

  void addImagePairFromBuffer(vector<char> *color_buffer, vector<char> *depth_buffer);
  void addImagePairFromFile(string color_path, string depth_path);
  void computeRigidTransform(Frame *other, float T[12], float rigidtrans[12]);
  void convert(int type);
};

#endif
