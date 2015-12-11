/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 * TODO
 * ----
 * - Fix process quit if error on one of the threads (e.g., connection dead)
 *
 ******************************************************************************/

#include "StackTrace.h"
#include "TCPServer.h"
#include "Device.h"
#include "Data.h"

#include "data_processors.h"
#include "data_writers.h"

//-----------------------------------------------------------------------------

// #include <ctime>
// #include "lib/CudaSift/cudaImage.h"
// #include "lib/CudaSift/RGBD_utils.h"
#include "Parameters.h"
// #include "Frame.h"

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  StackTrace stackTrace;

#ifdef REWRITE
  FILE *fp;
  Data *data = new Data();

  // fp = fopen("gold", "r+");
  // data.digest(fileno(fp));
  // fp = fopen("silver", "r+");
  // data.digest(fileno(fp));
  // fp = fopen("blue", "r+");
  // data.digest(fileno(fp));
  // fp = fopen("pink", "r+");
  // data.digest(fileno(fp));

  // Parameters *parameters = new Parameters(argv[1], argv[2]);
  Parameters *parameters = new Parameters("../", "data/");

  fp = fopen("iPhone", "r+");
  data->preprocessor = memory_preprocessor;
  data->processor = memory_processor;
  data->writer = memory_writer;
  
  // data->preprocessor = disk_preprocessor;
  // data->processor = disk_processor;
  // data->writer = disk_writer;

  data->parameters = parameters;
  data->digest(fileno(fp));

  delete data;
  fclose(fp);
  fp = NULL;

#else
  TCPServer *server = new TCPServer(8125);

  // server->add_device(new Device((char *)"gold", (char *)"192.168.0.105", 8124));
  // server->add_device(new Device((char *)"silver", (char *)"192.168.0.106", 8124));
  // server->add_device(new Device((char *)"blue", (char *)"192.168.0.107", 8124));
  // server->add_device(new Device((char *)"pink", (char *)"192.168.0.108", 8124));
  server->add_device(new Device((char *)"iPhone", (char *)"192.168.0.109", 8124));

  server->listen();
  server->connect();

  pthread_exit(NULL);
#endif

  return 0;
}

// vector<char> *readFileToBuffer(string path) {
//   // Open file
//   ifstream file(path, ios::binary);

//   // Seek to the end and find file size
//   file.seekg(0, ios::end);
//   streamsize size = file.tellg();

//   // Go back to beginning
//   file.seekg(0, ios::beg);

//   vector<char> *buffer = new vector<char>(size);

//   if (file.read(buffer->data(), size)) {
//     cout << "Reading file into buffer succeeded!" << endl;
//   } else {
//     cout << "Reading file into buffer failed..." << endl;
//   }

//   return buffer;
// }

// void align(Frame &lframe, Frame &rframe, Parameters *parameters) {
//   lframe.computeRigidTransform(&rframe, &parameters->extrinsic[12 * 0]);

//   cv::Mat color = cv::imread(parameters->color_list[0]);
//   WritePlyFile("../result/l.ply", lframe.pairs[0]->pointCloud, color);
//   color = cv::imread(parameters->color_list[1]);
//   WritePlyFile("../result/r.ply", rframe.pairs[0]->pointCloud, color);
// }

// void alignFromFile(Parameters *parameters) {
//   Frame lframe(parameters);
//   Frame rframe(parameters);

//   lframe.addImagePairFromFile(parameters->color_list[0], parameters->depth_list[0]);
//   rframe.addImagePairFromFile(parameters->color_list[1], parameters->depth_list[1]);

//   align(lframe, rframe, parameters);
// }

// void alignFromBuffer(Parameters *parameters) {
//   vector<char> *lcolor = readFileToBuffer(parameters->color_list[0]);
//   vector<char> *rcolor = readFileToBuffer(parameters->color_list[1]);
//   vector<char> *ldepth = readFileToBuffer(parameters->depth_list[0]);
//   vector<char> *rdepth = readFileToBuffer(parameters->depth_list[1]);

//   Frame lframe(parameters);
//   Frame rframe(parameters);

//   lframe.addImagePairFromBuffer(lcolor, ldepth);
//   rframe.addImagePairFromBuffer(rcolor, rdepth);

//   align(lframe, rframe, parameters);

//   delete lcolor;
//   delete rcolor;
//   delete ldepth;
//   delete rdepth;
// }