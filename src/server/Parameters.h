#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "lib/cuSIFT/RGBD_utils.h"

class Parameters {
public:
  vector<string> color_list;    /**< list of paths to color images*/
  vector<string> depth_list;    /**< list of paths to depth images */
  float* extrinsic;
  float* projection_d2c;
  int numofframe;
  Camera *color_camera;
  Camera *depth_camera;

  Parameters(const string, const string);
  ~Parameters();

  void readFromFile(const string, const string);
};

#endif
