#include "cudaImage.h"
#include "RGBD_utils.h"
#include "dataTrain.h"
#include <ctime>

cv::Mat PrintMatchData(SiftData &siftData1, SiftData &siftData2, cv::Mat limg, cv::Mat rimg)
{
  int numPts = siftData1.numPts;
  SiftPoint *sift1 = siftData1.h_data;
  cv::Mat im3(limg.size().height, limg.size().width + rimg.size().width, CV_32FC1);
  cv::Mat left(im3, cv::Rect(0, 0, limg.size().width, limg.size().height));
  limg.copyTo(left);
  cv::Mat right(im3, cv::Rect(limg.size().width, 0, rimg.size().width, rimg.size().height));
  rimg.copyTo(right);

  int w = limg.size().width + rimg.size().width;
  for (int j = 0; j < numPts; j++) {
    if (sift1[j].valid == 1) {
      float dx = sift1[j].match_xpos + limg.size().width - sift1[j].xpos;
      float dy = sift1[j].match_ypos - sift1[j].ypos;
      int len = (int)(fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy));
      for (int l = 0; l < len; l++) {
        int x = (int)(sift1[j].xpos + dx * l / len);
        int y = (int)(sift1[j].ypos + dy * l / len);
        im3.at<float>(y, x) = 255.0f;

      }
    }
  }
  //std::cout << std::setprecision(6);
  return im3;
}
void PrintMatchSiftData(SiftData &siftData1, const char* filename, int imgw) {
  ofstream fout(filename);
  if (!fout)
  {
    cout << "File Not Opened" << endl;  return;
  }
  SiftPoint *sift1 = siftData1.h_data;
  for (int i = 0; i < siftData1.numPts; i++)
  {
    if (sift1[i].valid) {
      int ind  = ((int)sift1[i].xpos + (int)sift1[i].ypos * imgw);
      int ind2 = ((int)sift1[i].match_xpos + (int)sift1[i].match_ypos * imgw);

      fout << sift1[i].xpos << "\t" << sift1[i].ypos << "\t";
      fout << sift1[i].match_xpos << "\t" << sift1[i].match_ypos << "\t";
      fout << ind << "\t" << ind2 << "\t";
      fout << endl;
    }

  }

  fout.close();

}

SiftData computeSift(cv::Mat inputImage) {
  inputImage.convertTo(inputImage, CV_32FC1);
  unsigned int w = inputImage.cols;
  unsigned int h = inputImage.rows;
  //std::cout << "Image size = (" << w << "," << h << ")" << std::endl;
  cv::GaussianBlur(inputImage, inputImage, cv::Size(5, 5), 1.0);

  //std::cout << "Initializing data..." << std::endl;
  InitCuda();
  CudaImage img1;
  img1.Allocate(w, h, iAlignUp(w, 128), false, NULL, (float*)inputImage.data);
  img1.Download();
  SiftData siftData1;
  float initBlur = 0.0f;
  float thresh = 3.0f;
  InitSiftData(siftData1, 2048, true, true);
  double timesift1 = ExtractSift(siftData1, img1, 5, initBlur, thresh, 0.0f);
  std::cout << "Extract sift time: " <<  timesift1 << std::endl;
  std::cout << "Number of original features: " <<  siftData1.numPts << std::endl;
  return siftData1;
}

int getMatch3dPoints(const SiftData siftData1, const cv::Mat pointCloud_l, const cv::Mat pointCloud_r,
                     cv::Mat* refCoord, cv::Mat* movCoord, int imgw, int imgh,
                     float minScore = 0.75f, float maxAmbiguity = 0.95f)
{
  int numTomatchedsift = 0;
  SiftPoint *sift1 = siftData1.h_data;
  for (int i = 0; i < siftData1.numPts; i++) {
    int ind  = ((int)sift1[i].xpos + (int)sift1[i].ypos * imgw);
    int ind2 = ((int)sift1[i].match_xpos + (int)sift1[i].match_ypos * imgw);
    if (sift1[i].ambiguity < maxAmbiguity && sift1[i].score > minScore
        && pointCloud_l.at<float>(ind, 2) > 0 && pointCloud_r.at<float>(ind2, 2) > 0) {

      //std::cout <<sift1[i].xpos<<"\t"<<sift1[i].ypos<<"\t"<<ind<<"\t"<<imgw<<std::endl;
      //std::cout<<pointCloud_l.at<float>(ind,0)<<","<<pointCloud_l.at<float>(ind,1)<<","<<pointCloud_l.at<float>(ind,2)<<std::endl;
      refCoord->push_back(pointCloud_l.row(ind));
      movCoord->push_back(pointCloud_r.row(ind2));
      numTomatchedsift++;
      sift1[i].valid = 1;
    }
  }
  return numTomatchedsift;
}

///////////////////////////////////////////////////////////////////////////////
// Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  // Read images using OpenCV
  string dataRoot = "../data";
  string sequenceName = "/";
  DataTrain dataTrain(dataRoot, sequenceName);

  //start to align two view
  int frame_i = 0;
  int frame_j = 1;

  // Get first two image paths
  string limg_file = dataTrain.color_list[frame_i];
  string rimg_file = dataTrain.color_list[frame_j];

  // Read image files
  cv::Mat limg = cv::imread(limg_file, 0);
  cv::Mat rimg = cv::imread(rimg_file, 0);
  cv::Mat ldepth = GetDepthData(dataTrain.depth_list[frame_i]);
  cv::Mat rdepth = GetDepthData(dataTrain.depth_list[frame_j]);

  cv::Mat pointCloud_l = depth2XYZcamera(dataTrain.camera, ldepth, 1);
  cv::Mat pointCloud_r = depth2XYZcamera(dataTrain.camera, rdepth, 1);

  // transform to world coordinate
  cv::Mat tmp = pointCloud_l;
  pointCloud_l = transformPointCloud(pointCloud_l, &dataTrain.extrinsic[12 * frame_i]);
  tmp.release();

  // Extract Sift features from images
  SiftData siftData1 = computeSift(limg);
  SiftData siftData2 = computeSift(rimg);

  // Match Sift features and find a homography
  MatchSiftData(siftData1, siftData2);

  // get the coordinate of matched points
  cv::Mat refCoord(0, 3, cv::DataType<float>::type);
  cv::Mat movCoord(0, 3, cv::DataType<float>::type);
  int imgw = rimg.size().width;
  int imgh = rimg.size().height;

  int numTomatchedsift = getMatch3dPoints(siftData1, pointCloud_l, pointCloud_r, &refCoord, &movCoord, imgw, imgh);
  std::cout << "numTomatchedsift:" <<  numTomatchedsift << std::endl;

  cv::Mat refCoord_t = refCoord;

  int numMatches[1];
  float rigidtrans[12];
  int numLoops = 1000;
  numLoops = ceil(numLoops / 128) * 128;

  ransacfitRt(refCoord_t, movCoord, rigidtrans, numMatches, numLoops, 0.1);


  cv::Mat pointCloud_r_t = transformPointCloud(pointCloud_r, rigidtrans);
  limg.convertTo(limg, CV_32FC1);
  rimg.convertTo(rimg, CV_32FC1);
  cv::Mat imRresult = PrintMatchData(siftData1, siftData2, limg, rimg);
  printf("write image\n");
  cv::imwrite("../result/imRresult_beforeransac.jpg", imRresult);
  imRresult.release();
  cv::Mat color = cv::imread(limg_file);
  WritePlyFile("../result/ref.ply", pointCloud_l, color);
  color = cv::imread(rimg_file);
  WritePlyFile("../result/mov.ply", pointCloud_r, color);
  color = cv::imread(rimg_file);
  WritePlyFile("../result/mov_afteralign.ply", pointCloud_r_t, color);
  pointCloud_r.release();
  // Print out and store summary data


  //std::cout << "Number of matching features: " << numFit << " " << numMatches << " " << 100.0f*numMatches/std::min(siftData1.numPts, siftData2.numPts) << "%" << std::endl;

  // Free Sift data from device
  FreeSiftData(siftData1);
  FreeSiftData(siftData2);

  movCoord.release();
  refCoord.release();
  limg.release();
  rimg.release();

  return 1;
}


