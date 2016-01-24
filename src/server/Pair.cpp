#include "Pair.h"

int Pair::currIndex = 0;

Pair::Pair(vector<char> *color_buffer, vector<char> *depth_buffer) {
  cv::Mat gray = cv::imdecode(*color_buffer, cv::IMREAD_GRAYSCALE);
  pointCloud = new PointCloud(color_buffer, depth_buffer);

  initPair(gray);
}

Pair::Pair(string color_path, string depth_path) {
  cv::Mat gray = cv::imread(color_path, cv::IMREAD_GRAYSCALE);
  pointCloud = new PointCloud(color_path, depth_path);

  initPair(gray);
}

Pair::~Pair() {
  FreeSiftData(siftData);

  if (pointCloud != NULL) {
    delete pointCloud;
  }
}

void Pair::initPair(cv::Mat gray) {
  pair_index = Pair::currIndex++;
  width = gray.cols;
  height = gray.rows;
  // computeSift(gray);
  string path = "../result/sift/sift" + to_string(currIndex);
  ReadVLFeatSiftData(siftData, path.c_str());
}

void Pair::computeSift(cv::Mat gray) {

  // Convert grayscale image to 32-bit float with 1 channel
  gray.convertTo(gray, CV_32FC1);

  // Blur image
  cv::GaussianBlur(gray, gray, cv::Size(5, 5), 1.0);

  // Initializing image data onto GPU
  InitCuda();
  cuImage cudaImage;
  cudaImage.Allocate(width, height, iAlignUp(width, 128), false, NULL, (float*) gray.data);
  cudaImage.Download();

  // Initialize SIFT data on both host and device
  float initBlur = 0.0f;
  float thresh = 2.0f;
  InitSiftData(siftData, 2048, true, true);

  // Extract sift data
  ExtractRootSift(siftData, cudaImage, 6, initBlur, thresh, 0.0f);
  
  gray.release();
}

// Copy over 3D data for sift points
void Pair::computeSift3D() {
  for (int i = 0; i < siftData.numPts; i++) {
    SiftPoint *point = siftData.h_data + i;
    int idx = ((int)point->coords2D[0]) + ((int)point->coords2D[1]) * width;
    memcpy(point->coords3D, (float *)pointCloud->depth.row(idx).data, sizeof(float) * 3);
  }
}

void Pair::deletePointCloud() {
  delete pointCloud;
  pointCloud = NULL;
}
