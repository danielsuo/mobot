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

  if (pointCloud != nullptr) {
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
  cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0.5);

  // Initializing image data onto GPU
  // TODO: only need to initcuda once
  InitCuda();
  cuImage cudaImage;
  cudaImage.Allocate(width, height, iAlignUp(width, 128), false, NULL, (float*) gray.data);
  cudaImage.Download();

  // Initialize SIFT data on both host and device
  float initBlur = 0.0f;
  float thresh = 0.1f;
  InitSiftData(siftData, 2048, true, true);

  // Extract sift data
  ExtractRootSift(siftData, cudaImage, 6, initBlur, thresh, 0.0f);
  
  gray.release();
}

// Copy over 3D data for sift points
void Pair::computeSift3D() {
  for (int i = 0; i < siftData.numPts; i++) {
    SiftPoint *point = siftData.h_data + i;

    int x = ((int)round(point->coords2D[0]));
    int y = ((int)round(point->coords2D[1]));
    int idx = x + y * width;

    if (x < width && y < height && x >= 0 && y >= 0 && pointCloud->depth.at<float>(idx, 2) != 0) {

      point->coords3D[0] = pointCloud->depth.at<float>(idx, 0);
      point->coords3D[1] = pointCloud->depth.at<float>(idx, 1);
      point->coords3D[2] = pointCloud->depth.at<float>(idx, 2);
      // memcpy(point->coords3D, (float *)pointCloud->depth.row(idx).data, sizeof(float) * 3);
    } else {
      point->coords3D[0] = 0;
      point->coords3D[1] = 0;
      point->coords3D[2] = 0;
    }
  }
}

void Pair::deletePointCloud() {
  delete pointCloud;
  pointCloud = nullptr;
}
