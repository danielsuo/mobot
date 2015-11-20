#include "RGBD_utils.h"
////////////////////////////////////////////////////////////////////////////////

//outputPly(PLYfilename, cameraRtC2W, data,Scale)
void WritePlyFile(const char* plyfile, const cv::Mat pointCloud, const cv::Mat color){
	FILE *fp = fopen(plyfile,"w");
	int pointCount =0;
	for (int v = 0; v < pointCloud.size().height; ++v) {
		if (pointCloud.at<float>(v,2)>0.0001){
			pointCount++;
		}
	}

  cout << "Write PLY" << endl;

  fprintf(fp, "ply\n");
  fprintf(fp, "format binary_little_endian 1.0\n");
  fprintf(fp, "element vertex %d\n", pointCount);
  fprintf(fp, "property float x\n");
  fprintf(fp, "property float y\n");
  fprintf(fp, "property float z\n");
  fprintf(fp, "property uchar red\n");
  fprintf(fp, "property uchar green\n");
  fprintf(fp, "property uchar blue\n");
  fprintf(fp, "end_header\n");

  for (int v = 0; v < pointCloud.size().height; ++v) {
    if (pointCloud.at<float>(v,2)>0.0001){
      fwrite(&pointCloud.at<float>(v,0), sizeof(float), 1, fp);
      fwrite(&pointCloud.at<float>(v,1), sizeof(float), 1, fp);
      fwrite(&pointCloud.at<float>(v,2), sizeof(float), 1, fp);
      int i= (int)v/color.size().width;
      int j= (int)v%color.size().width;
      fwrite(&color.at<cv::Vec3b>(i,j)[2], sizeof(uchar), 1, fp);
      fwrite(&color.at<cv::Vec3b>(i,j)[1], sizeof(uchar), 1, fp);
      fwrite(&color.at<cv::Vec3b>(i,j)[0], sizeof(uchar), 1, fp);
    }
  }
  fclose(fp);
}

void writeMatToFile(cv::Mat& m, const char* filename)
{
  ofstream fout(filename);

  double sum = 0;

  if(!fout) {
    cout<<"File Not Opened"<<endl;  return;
  }

  for(int i = 0; i < m.rows; i++) {
    for(int j = 0; j < m.cols; j++) {
      fout << m.at<float>(i, j) << "\t";
      sum += m.at<float>(i, j);
    }
    fout << endl;
  }

  fprintf(stderr, "Sum of matrix: %f\n", sum);

  fout.close();
}

void ransacfitRt(const cv::Mat refCoord, const cv::Mat movCoord, float* rigidtransform, 
 int* numMatches, int numLoops, float thresh)
{
  cv::Mat coord(refCoord.size().height, refCoord.size().width+movCoord.size().width, CV_32FC1);
  cv::Mat left(coord, cv::Rect(0, 0, refCoord.size().width, refCoord.size().height));
  refCoord.copyTo(left);
  cv::Mat right(coord, cv::Rect(refCoord.size().width, 0, movCoord.size().width, movCoord.size().height));
  movCoord.copyTo(right);
  float * h_coord = (float*)coord.data;
  //writeMatToFile(coord, "h_coord.txt");
  int numValid = refCoord.size().height;
  
  int randSize = 4*sizeof(int)*numLoops;
  int* h_randPts = (int*)malloc(randSize);

  // generate random samples for each loop
  for (int i=0;i<numLoops;i++) {
    int p1 = rand() % numValid;
    int p2 = rand() % numValid;
    int p3 = rand() % numValid;
    while (p2==p1) p2 = rand() % numValid;
    while (p3==p1 || p3==p2) p3 = rand() % numValid;
    h_randPts[i+0*numLoops] = p1;
    h_randPts[i+1*numLoops] = p2;
    h_randPts[i+2*numLoops] = p3;
  }

  int h_count =-1;
  float thresh2 = thresh*thresh;

#ifdef CPURANSAC
  float h_RT[12];
  int maxIndex = -1;
  int maxCount = -1;
  for(int idx= 0;idx<numLoops;idx++){

    estimateRigidTransform(h_coord, h_randPts, idx, numLoops, h_RT);
    TestRigidTransform(h_coord, h_RT, &h_count, numValid, thresh2);

    if (h_count>maxCount){
      maxCount = h_count;
      for (int i = 0;i<12;i++){
        rigidtransform[i] = h_RT[i];
      }
    }
  }

  numMatches[0] = maxCount;

#endif

  // gpu ransac;
  gpuRASACfindRT(h_coord,h_randPts,rigidtransform,numMatches,numLoops,numValid,thresh2);
#ifdef VERBOSE
  cout << endl;
  cout << "RANSAC Fit Rt" << endl;

  for (int i = 0; i < 12; i++) {
    fprintf(stderr, "%0.4f ", rigidtransform[i]);
    if ((i + 1) % 4 == 0) cout << endl;
  }
  cout << endl;
  printf("Num loops: %d\n", numLoops);
  printf("Threshold %0.4f\n", thresh);

  printf("numofMatch = %d \n",numMatches[0]);
#endif
  // printf("rigidtransform\n");
  // for (int jj =0; jj<3;jj++){
  //     printf("%f,%f,%f,%f\n",rigidtransform[0+jj*4],rigidtransform[1+4*jj],rigidtransform[2+4*jj],rigidtransform[3+4*jj]);
  // }

  free(h_randPts);

  return;
}

unsigned int uchar2uint(unsigned char* in) {
  return (((unsigned int)(in[0])) << 16) + (((unsigned int)(in[1])) << 8) + ((unsigned int)(in[2]));
}

void uint2uchar(unsigned int in, unsigned char* out) {
  out[0] = (in & 0x00ff0000) >> 16;
  out[1] = (in & 0x0000ff00) >> 8;
  out[2] = in & 0x000000ff;
}

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
