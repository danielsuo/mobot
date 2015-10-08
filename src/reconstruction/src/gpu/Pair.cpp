#include "Pair.h"

Pair::Pair(string color_path, string depth_path, Camera camera) {
  color = cv::imread(color_path, cv::IMREAD_GRAYSCALE); // Set flag to convert any image to grayscale
  depth = cv::imread(depth_path, cv::IMREAD_ANYDEPTH);

  processDepth();
  createPointCloud(camera);
  computeSift();
}

Pair::~Pair() {
  // TODO: need to figure out how to hold onto and deallocate
  // resources
  // FreeSiftData(siftData);
  // release match coordinates
  // release images
  // release point cloud
}

void Pair::processDepth() {
  cv::Mat result(depth.rows, depth.cols, cv::DataType<float>::type);

  for (int i = 0; i < depth.rows; i++) {
    for (int j = 0; j < depth.cols; j++) {
      unsigned short s = depth.at<ushort>(i, j);

      // In order to visually see depth, we bit shift during
      // capture. Now we must shift back.
      s = (s << 13 | s >> 3);
      float f = (float)s / 1000.0;
      result.at<float>(i, j) = f;
    }
  }

  depth = result;
}

void Pair::createPointCloud(Camera camera) {
  // Initialize 3 dimensions for each pixel in depth image
  cv::Mat result(depth.size().height * depth.size().width, 3, cv::DataType<float>::type);

  for (int v = 0; v < depth.size().height; v++) {
    for (int u = 0; u < depth.size().width; u++) {

      float iz = depth.at<float>(v,u);
      float ix = iz * (u - camera.cx) / camera.fx;
      float iy = iz * (v - camera.cy) / camera.fy;

      result.at<float>(v * depth.size().width+u, 0) = ix;
      result.at<float>(v * depth.size().width+u, 1) = iy;
      result.at<float>(v * depth.size().width+u, 2) = iz;
    }
  }

  pointCloud = result;
}

void Pair::transformPointCloud(float T[12]) {
  cv::Mat result(pointCloud.size().height,3,cv::DataType<float>::type);
  for (int v = 0; v < pointCloud.size().height; ++v) {
    float ix = pointCloud.at<float>(v,0);
    float iy = pointCloud.at<float>(v,1);
    float iz = pointCloud.at<float>(v,2);

    result.at<float>(v,0) = T[0] * ix + T[1] * iy + T[2] * iz + T[3];
    result.at<float>(v,1) = T[4] * ix + T[5] * iy + T[6] * iz + T[7];
    result.at<float>(v,2) = T[8] * ix + T[9] * iy + T[10] * iz + T[11];
  }

  pointCloud = result;
}

void Pair::computeSift() {
  color.convertTo(color, CV_32FC1);
  unsigned int w = color.cols;
  unsigned int h = color.rows;
  cout << "Image size = (" << w << "," << h << ")" <<endl;
  cv::GaussianBlur(color, color, cv::Size(5, 5), 1.0);

  cout << "Initializing data..." << endl;
  InitCuda();
  CudaImage cudaImage;
  cudaImage.Allocate(w, h, iAlignUp(w, 128), false, NULL, (float*)color.data);
  cudaImage.Download();

  float initBlur = 0.0f;
  float thresh = 3.0f;
  InitSiftData(siftData, 2048, true, true);
  double timesift1 = ExtractSift(siftData, cudaImage, 5, initBlur, thresh, 0.0f);
  cout << "Extract sift time: " <<  timesift1 << endl;
  cout << "Number of original features: " << siftData.numPts << endl;
}

int Pair::getMatched3DPoints(Pair &other, cv::Mat &lmatch, cv::Mat &rmatch) {
  MatchSiftData(siftData, other.siftData);

  float minScore = 0.75f;
  float maxAmbiguity = 0.95f;
  int numToMatchedSift = 0;
  int imgw = color.cols;

  SiftPoint *siftPoint = siftData.h_data;
  for(int i = 0; i < siftData.numPts; i++) {
    int index_self = ((int)siftPoint[i].xpos + (int)siftPoint[i].ypos * imgw);
    int index_other = ((int)siftPoint[i].match_xpos + (int)siftPoint[i].match_ypos * imgw);

    if (siftPoint[i].ambiguity < maxAmbiguity &&
        siftPoint[i].score > minScore &&
        pointCloud.at<float>(index_self, 2) > 0 &&
        other.pointCloud.at<float>(index_other, 2) > 0) {
      lmatch.push_back(pointCloud.row(index_self));
      rmatch.push_back(other.pointCloud.row(index_other));

      numToMatchedSift++;
      siftPoint[i].valid = 1;
    }
  }

  return numToMatchedSift;
}

void Pair::convert(int type) {
  color.convertTo(color, type);
}
