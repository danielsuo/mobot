/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include "debug.h"

void testBitShiftDepth();

int main(int argc, char *argv[]) {
  cerr << "Called with " << argc << endl;

  for (int i = 0; i < argc; i++) {
    cerr << "Arg " << i << ": " << argv[i] << endl;
  }

  // testBitShiftDepth();

  readDataFromBlobToMemory();
  // readDataFromBlobToDisk(argc, argv);
  // testCUBOFCreate();
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

void testBitShiftDepth() {
  PointCloud *pointCloud = new PointCloud();

  pointCloud->color = cv::imread("../lib/cuBoF/lib/cuSIFT/test/data/color1.jpg", cv::IMREAD_COLOR);
  pointCloud->depth = cv::imread("../lib/cuBoF/lib/cuSIFT/test/data/depth1.png", cv::IMREAD_ANYDEPTH);

  int numZeros = 0;
  double sum = 0;

  for (int r = 0; r < pointCloud->depth.rows; r++) {
    for (int c = 0; c < pointCloud->depth.cols; c++) {
      if (pointCloud->depth.at<uint16_t>(r, c) == 0) {
        numZeros++;
      } else {
        // cerr << pointCloud->depth.at<uint16_t>(r, c) << " ";
        sum += pointCloud->depth.at<uint16_t>(r, c);
      }
    }
  }

  cerr << "Number of zeros before bitshift: " << numZeros << endl;
  cerr << "Sum before bitshift: " << sum << endl;

  // pointCloud->bitShiftDepth();

  // numZeros = 0;
  // sum = 0;

  // for (int r = 0; r < pointCloud->depth.rows; r++) {
  //   for (int c = 0; c < pointCloud->depth.cols; c++) {
  //     if (pointCloud->depth.at<float>(r, c) == 0.0f) {
  //       numZeros++;
  //     } else {
  //       sum += pointCloud->depth.at<float>(r, c);
  //     }
  //   }
  // }

  // cerr << "Number of zeros after bitshift: " << numZeros << endl;
  // cerr << "Sum after bitshift: " << sum << endl;

  pointCloud->createPointCloud();

  numZeros = 0;
  sum = 0;

  for (int r = 0; r < pointCloud->depth.rows; r++) {
    if (pointCloud->depth.at<float>(r, 2) == 0.0f) {
      numZeros++;
    } else {
      sum += pointCloud->depth.at<float>(r, 0);
      sum += pointCloud->depth.at<float>(r, 1);
      sum += pointCloud->depth.at<float>(r, 2);
    }
  }

  cerr << "Number of zeros after pointcloud: " << numZeros << endl;
  cerr << "Sum after pointcloud: " << sum << endl;
}

void *mobotDemoFriend(void *pointer) {
  
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

