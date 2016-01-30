/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include "debug.h"


int main(int argc, char *argv[]) {
  // testCUBOFCreate();
  readDataFromBlobToMemory();
  // compareIndices();
  // testBundleAdjustment(argc, argv);

  // buildGrid();
  // testMobot();
  // testRounding();
  // testReadPLY();

  // initializeMobot();
  // pthread_t tmp;
  // pthread_create(&tmp, NULL, mobotDemo, NULL);
  // mobotDemo(NULL);
  // readDataFromTCPToBlob();
  // readDataFromTCPToMemory();

  // pthread_exit(NULL);
  return 0;
}

void testReadPLY() {


  // cv::Mat tmp = PointCloud::readPLY("../result/merged.ply");

  cv::Mat tmp(10, 3, cv::DataType<float>::type);

  for (int i = 0; i < 10; i++) {
    tmp.at<float>(i, 0) = i;
    tmp.at<float>(i, 1) = 1;
    tmp.at<float>(i, 2) = 1;
  }

  Grid *grid = Grid::gridFromMat(tmp);
  // cv::Mat filtered(0, 3, cv::DataType<float>::type);

  // for (int i = 0; i < tmp.size().height; i++) {
  //   if (tmp.at<float>(i, 0) * tmp.at<float>(i, 0) + tmp.at<float>(i, 2) * tmp.at<float>(i, 2) < 9) {
  //     filtered.push_back(tmp.row(i));
  //   }
  // }

  // PointCloud::writePLY("../result/filtered.ply", filtered);

  // float minx, maxx, miny, maxy, minz, maxz;
  // minx = miny = minz = FLT_MAX;
  // maxx = maxy = maxz = FLT_MIN;

  // for (int v = 0; v < filtered.size().height; v++) {
  //   if (filtered.at<float>(v, 2) != 0) {
  //     float ix = filtered.at<float>(v, 0);
  //     float iy = filtered.at<float>(v, 1);
  //     float iz = filtered.at<float>(v, 2);

  //     if (ix < minx) minx = ix;
  //     else if (ix > maxx) maxx = ix;
  //     if (iy < miny) miny = iy;
  //     else if (iy > maxy) maxy = iy;
  //     if (iz < minz) minz = iz;
  //     else if (iz > maxz) maxz = iz; 
  //   }
  // }

  // fprintf(stderr, "x (%0.4f, %0.4f), y (%0.4f, %0.4f), z (%0.4f, %0.4f)\n", minx, maxx, miny, maxy, minz, maxz);
}

