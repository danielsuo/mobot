#include <ctime>
#include "cudaImage.h"
#include "RGBD_utils.h"
#include "Parameters.h"
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

void align(Frame &lframe, Frame &rframe, Parameters *parameters) {
  float rigidtrans[12];
  lframe.computeRigidTransform(&rframe, &parameters->extrinsic[12 * 0], rigidtrans);

  cv::Mat color = cv::imread(parameters->color_list[0]);
  WritePlyFile("../result/l.ply", lframe.pairs[0]->pointCloud, color);
  color = cv::imread(parameters->color_list[1]);
  WritePlyFile("../result/r.ply", rframe.pairs[0]->pointCloud, color);
}

void alignFromFile(Parameters *parameters) {
  Frame lframe(parameters);
  Frame rframe(parameters);

  lframe.addImagePairFromFile(parameters->color_list[0], parameters->depth_list[0]);
  rframe.addImagePairFromFile(parameters->color_list[1], parameters->depth_list[1]);

  align(lframe, rframe, parameters);
}

void alignFromBuffer(Parameters *parameters) {
  vector<char> *lcolor = readFileToBuffer(parameters->color_list[0]);
  vector<char> *rcolor = readFileToBuffer(parameters->color_list[1]);
  vector<char> *ldepth = readFileToBuffer(parameters->depth_list[0]);
  vector<char> *rdepth = readFileToBuffer(parameters->depth_list[1]);

  Frame lframe(parameters);
  Frame rframe(parameters);

  lframe.addImagePairFromBuffer(lcolor, ldepth);
  rframe.addImagePairFromBuffer(rcolor, rdepth);

  align(lframe, rframe, parameters);

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
  Parameters parameters(argv[1], argv[2]);

  // alignFromFile(datatrain);
  alignFromBuffer(&parameters);

  // Pair test(parameters.color_list[0], parameters.depth_list[0], &parameters);


  return 1;
}

