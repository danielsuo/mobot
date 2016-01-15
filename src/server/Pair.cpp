#include "Pair.h"

int Pair::currIndex = 0;

Pair::Pair(vector<char> *color_buffer, vector<char> *depth_buffer) {
  gray = cv::imdecode(*color_buffer, cv::IMREAD_GRAYSCALE);
  pointCloud = new PointCloud(color_buffer, depth_buffer);

  initPair();
}

Pair::Pair(string color_path, string depth_path) {
  gray = cv::imread(color_path, cv::IMREAD_GRAYSCALE);
  pointCloud = new PointCloud(color_path, depth_path);

  initPair();
}

Pair::~Pair() {
  FreeSiftData(siftData);

  gray.release();

  if (pointCloud != NULL) {
    delete pointCloud;
  }
}

void Pair::initPair() {
  pair_index = Pair::currIndex++;
  computeSift();
}

void Pair::computeSift() {

  // Convert grayscale image to 32-bit float with 1 channel
  gray.convertTo(gray, CV_32FC1);

  // Get width and height of image
  unsigned int w = gray.cols;
  unsigned int h = gray.rows;

  // Blur image
  cv::GaussianBlur(gray, gray, cv::Size(5, 5), 1.0);

  // Initializing image data onto GPU
  InitCuda();
  CudaImage cudaImage;
  cudaImage.Allocate(w, h, iAlignUp(w, 128), false, NULL, (float*) gray.data);
  cudaImage.Download();

  // Initialize SIFT data on both host and device
  float initBlur = 0.0f;
  float thresh = 2.0f;
  InitSiftData(siftData, 2048, true, true);

  // Extract sift data
  ExtractRootSift(siftData, cudaImage, 6, initBlur, thresh, 0.0f);

  // std::ostringstream siftDataPath;
  // siftDataPath << "../result/sift/sift";
  // siftDataPath << frame_index + 1;
  // ReadVLFeatSiftData(siftData, siftDataPath.str().c_str());

  // PrintSiftData(siftDataTest);

  // std::ostringstream imresult_path;
  // imresult_path << "../result/imRresult_beforeransac_";
  // imresult_path << index;
  // imresult_path << ".jpg";
  // cv::imwrite(imresult_path.str().c_str(), imRresult);

  // FreeSiftData(siftDataTest);
  // SiftPoint *siftPoints = siftData.h_data;

  // for (int i = 0; i < siftData.numPts; i++) {
  //   fprintf(stderr, "Siftpoint (x, y): (%d, %d)\n", (int)siftPoints[i].xpos, (int)siftPoints[i].ypos);
  //   for (int j = 0; j < 128; j++) {
  //     fprintf(stderr, "%0.4f ", siftPoints[i].data[j]);
  //   }
  //   fprintf(stderr, "\n");
  // }

  fprintf(stderr, "Number of original features: %d\n", siftData.numPts);
}

void Pair::deletePointCloud() {
  delete pointCloud;
  pointCloud = NULL;
}

int Pair::getMatched3DPoints(Pair *other, cv::Mat &lmatch, cv::Mat &rmatch) {
  MatchSiftData(siftData, other->siftData, MatchSiftDistanceL2);
  fprintf(stderr, "Num matched sift points %d\n", siftData.numPts);

  float maxScore = 2 - 2 * 0.85f;
  float maxAmbiguity = 0.36f; // Ratio testing: 0.6^2
  // float minScore = 0.85f;
  // float maxAmbiguity = 0.95;

  int numToMatchedSift = 0;
  int imgw = gray.cols;
  int imgh = gray.rows;

  SiftPoint *siftPoints = siftData.h_data;
  for(int i = 0; i < siftData.numPts; i++) {
    int index_self = ((int)siftPoints[i].xpos + (int)siftPoints[i].ypos * imgw);
    int index_other = ((int)siftPoints[i].match_xpos + (int)siftPoints[i].match_ypos * imgw);

    if (siftPoints[i].ambiguity < maxAmbiguity &&
        // siftPoints[i].score > minScore &&
        siftPoints[i].score < maxScore &&
        pointCloud->depth.at<float>(index_self, 2) != 0 &&
        other->pointCloud->depth.at<float>(index_other, 2) != 0
        ) {
      lmatch.push_back(pointCloud->depth.row(index_self));
      rmatch.push_back(other->pointCloud->depth.row(index_other));

      numToMatchedSift++;
      siftPoints[i].valid = 1;
    } else {
      fprintf(stderr, "Rejected: (%d, %d) ratio %0.4f, score %0.4f\n", index_self, index_other, siftPoints[i].ambiguity, siftPoints[i].score);
    }
  }

  return numToMatchedSift;
}

void Pair::convert(int type) {
  gray.convertTo(gray, type);
}
