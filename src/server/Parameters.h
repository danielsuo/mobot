#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

typedef struct Camera {
  float cx,cy,fx,fy;
  float Kdepth[3][3];
  float R_d2c[3][3];
  float T_d2c[3];
} Camera;

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
