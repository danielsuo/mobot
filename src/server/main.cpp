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
#include "Parser.h"

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
  Parser *parser = new Parser();

  #ifdef MEMORY
  // Parameters *parameters = new Parameters(argv[1], argv[2]);
  Parameters *parameters = new Parameters("../", "data/");

  fp = fopen("iPhone", "r+");
  
  parser->preprocessor = memory_preprocessor;
  parser->processor = memory_processor;
  parser->writer = memory_writer;

  parser->parameters = parameters;
  parser->digest(fileno(fp));
  
  #elif DISK

  parser->preprocessor = disk_preprocessor;
  parser->processor = disk_processor;
  parser->writer = disk_writer;

  // gold
  fp = fopen("device3", "r+");
  parser->digest(fileno(fp));

  // silver
  fp = fopen("device2", "r+");
  parser->digest(fileno(fp));

  // blue
  fp = fopen("device4", "r+");
  parser->digest(fileno(fp));

  // pink
  fp = fopen("device1", "r+");
  parser->digest(fileno(fp));

  // fp = fopen("iPhone", "r+");
  // parser->digest(fileno(fp));

  #endif


  delete parser;
  fclose(fp);
  fp = NULL;

#else
  TCPServer *server = new TCPServer(8124);

  server->add_device(new Device((char *)"device3", (char *)"192.168.0.105", 8124));
  server->add_device(new Device((char *)"device2", (char *)"192.168.0.106", 8124));
  server->add_device(new Device((char *)"device4", (char *)"192.168.0.107", 8124));
  server->add_device(new Device((char *)"device1", (char *)"192.168.0.108", 8124));
  // server->add_device(new Device((char *)"iPhone", (char *)"192.168.0.109", 8124));

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