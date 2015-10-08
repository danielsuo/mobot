#include <ctime>
#include "cudaImage.h"
#include "RGBD_utils.h"
#include "DataTrain.h"
#include "Frame.h"

void alignFromFile(string directory, string sequence) {

  DataTrain dataTrain(directory, sequence);

  Frame lframe;
  Frame rframe;

  lframe.addImagePairFromFile(dataTrain.color_list[0], dataTrain.depth_list[0], dataTrain.camera);
  rframe.addImagePairFromFile(dataTrain.color_list[1], dataTrain.depth_list[1], dataTrain.camera);

  float rigidtrans[12];
  lframe.computeRigidTransform(&rframe, &dataTrain.extrinsic[12 * 0], rigidtrans);

  cv::Mat color = cv::imread(dataTrain.color_list[0]);
  WritePlyFile("../result/l.ply", lframe.pairs[0]->pointCloud, color);
  color = cv::imread(dataTrain.color_list[1]);
  WritePlyFile("../result/r.ply", rframe.pairs[0]->pointCloud, color);
}


//   int numMatches[1];
//   float rigidtrans[12];
//   int numLoops = 1000;
//   numLoops = ceil(numLoops / 128) * 128;

//   ransacfitRt(refCoord, movCoord, rigidtrans, numMatches, numLoops, 0.1);

//   cv::Mat pointCloud_r_t = transformPointCloud(pointCloud_r, rigidtrans);
//   limg.convertTo(limg, CV_32FC1);
//   rimg.convertTo(rimg, CV_32FC1);
//   cv::Mat imRresult = PrintMatchData(siftData1, siftData2, limg, rimg);
//   printf("write image\n");
//   cv::imwrite("../result/imRresult_beforeransac.jpg", imRresult);
//   imRresult.release();
//   cv::Mat color = cv::imread(limg_file);
//   WritePlyFile("../result/ref.ply", pointCloud_l, color);
//   color = cv::imread(rimg_file);
//   WritePlyFile("../result/mov.ply", pointCloud_r, color);
//   color = cv::imread(rimg_file);
//   WritePlyFile("../result/mov_afteralign.ply", pointCloud_r_t, color);
//   pointCloud_r.release();
//   // Print out and store summary data


//   //std::cout << "Number of matching features: " << numFit << " " << numMatches << " " << 100.0f*numMatches/std::min(siftData1.numPts, siftData2.numPts) << "%" << std::endl;

//   // Free Sift data from device
//   FreeSiftData(siftData1);
//   FreeSiftData(siftData2);

//   movCoord.release();
//   refCoord.release();
//   limg.release();
//   rimg.release();
// }

///////////////////////////////////////////////////////////////////////////////
// Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  alignFromFile(argv[1], argv[2]);

  return 1;
}

