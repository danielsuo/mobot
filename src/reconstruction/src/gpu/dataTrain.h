#include "RGBD_utils.h"

void readTraindata(const string dataRoot, const string sequenceName,
                   vector<string> &color_list, vector<string>& depth_list,
                   float* &extrinsic, int* numofframe, cameraModel &cam_K);
class DataTrain {
public:
  vector<string> color_list;
  vector<string> depth_list;
  float* extrinsic;
  int numofframe;
  cameraModel cameraModel;
  DataTrain(const string, const string);
};
DataTrain::DataTrain(const string dataRoot, const string sequenceName) {
  readTraindata(dataRoot, sequenceName, color_list, depth_list, extrinsic, &numofframe, cameraModel);
}


void readTraindata(const string dataRoot, const string sequenceName,
                   vector<string> &color_list, vector<string>& depth_list,
                   float* &extrinsic, int* numofframe, cameraModel &cam_K) {
  // color
  string listfile_color = dataRoot + sequenceName + "colorTrain.txt";
  string line;
  ifstream file_color (listfile_color);
  if (file_color.is_open()) {
    getline(file_color, line);
    *numofframe = atoi( line.c_str() );
    while (getline(file_color, line)) {
      color_list.push_back(line);
    }
    file_color.close();
  }
  else cout << "Unable to open file: " << listfile_color;


  //depth
  string listfile_depth = dataRoot + sequenceName + "depthTrain.txt";
  ifstream  myfile(listfile_depth);
  if (myfile.is_open()) {
    getline(myfile, line);
    while (getline(myfile, line)) {
      depth_list.push_back(line);
    }
    myfile.close();
  }
  else cout << "Unable to open file: " << listfile_color;
  //cout<< "numofframe : "<<numofframe[0]<<endl;
  //cout<<" color List :"<< color_list.size()<<endl;
  //cout<<" depth List :"<<depth_list.size()<<endl;

  // extrinsic
  string extrinsic_file = dataRoot + sequenceName + "extrinsics.txt";
  fstream ex_myfile(extrinsic_file);
  if (ex_myfile.is_open()) {
    extrinsic = (float*) malloc (sizeof(float) * numofframe[0] * 12);
    //int result = fread (extrinsic,sizeof(float),numofframe[0]*12,pFile);
    //fclose(pFile);
    for (int i = 0; i < 12 * numofframe[0]; i++) {
      ex_myfile >> extrinsic[i];
    }
  }
  //
  string cam_file = dataRoot + sequenceName + "/intrinsics.txt";
  ifstream  cam_myfile(cam_file);
  float tmp;
  if (cam_myfile.is_open()) {
    cam_myfile >> cam_K.fx;
    cam_myfile >> tmp;
    cam_myfile >> cam_K.cx;
    cam_myfile >> tmp;
    cam_myfile >> cam_K.fy;
    cam_myfile >> cam_K.cy;
    cam_myfile.close();
    //cout << cam_K.fx <<","<< cam_K.fy <<","<<cam_K.cx<<","<<cam_K.cy<<endl;
  }
  else cout << "Unable to open file: " << cam_file;
  return;
}