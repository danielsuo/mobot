#include <ctime>
#include "cudaImage.h"
#include "RGBD_utils.h"
#include "DataTrain.h"
#include "Frame.h"

vector<char> *readFileToBuffer(string path) {
  // Open file
  ifstream file(path, ios::binary);

  // Seek to the end and find file size
  file.seekg(0, ios::end);
  streamsize size = file.tellg();

  // Go back to beginning
  file.seekg(0, ios::beg);

  vector<char> *buffer = new vector<char>(size);

  if (file.read(buffer->data(), size)) {
    cout << "Reading file into buffer succeeded!" << endl;
  } else {
    cout << "Reading file into buffer failed..." << endl;
  }

  return buffer;
}

void align(Frame &lframe, Frame &rframe, DataTrain dataTrain) {
  float rigidtrans[12];
  lframe.computeRigidTransform(&rframe, &dataTrain.extrinsic[12 * 0], rigidtrans);

  cv::Mat color = cv::imread(dataTrain.color_list[0]);
  WritePlyFile("../result/l.ply", lframe.pairs[0]->pointCloud, color);
  color = cv::imread(dataTrain.color_list[1]);
  WritePlyFile("../result/r.ply", rframe.pairs[0]->pointCloud, color);
}

void alignFromFile(DataTrain dataTrain) {
  Frame lframe;
  Frame rframe;

  lframe.addImagePairFromFile(dataTrain.color_list[0], dataTrain.depth_list[0], dataTrain.camera);
  rframe.addImagePairFromFile(dataTrain.color_list[1], dataTrain.depth_list[1], dataTrain.camera);

  align(lframe, rframe, dataTrain);
}

void alignFromBuffer(DataTrain dataTrain) {
  vector<char> *lcolor = readFileToBuffer(dataTrain.color_list[0]);
  vector<char> *rcolor = readFileToBuffer(dataTrain.color_list[1]);
  vector<char> *ldepth = readFileToBuffer(dataTrain.depth_list[0]);
  vector<char> *rdepth = readFileToBuffer(dataTrain.depth_list[1]);

  Frame lframe;
  Frame rframe;

  lframe.addImagePairFromBuffer(lcolor, ldepth, dataTrain.camera);
  rframe.addImagePairFromBuffer(rcolor, rdepth, dataTrain.camera);

  align(lframe, rframe, dataTrain);

  delete lcolor;
  delete rcolor;
  delete ldepth;
  delete rdepth;
}

///////////////////////////////////////////////////////////////////////////////
// Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  DataTrain dataTrain(argv[1], argv[2]);

  // alignFromFile(datatrain);
  alignFromBuffer(dataTrain);


  return 1;
}

