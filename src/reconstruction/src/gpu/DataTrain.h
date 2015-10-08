/**
 * @file   dataTrain.h
 * @author Jianxiong Xiao <xj@princeton.edu>
 * @date   Tue Oct  6 21:46:53 2015
 *
 * @brief Class that holds training data. Constructor calls function
 * that reads in data.
 *
 */

#include "RGBD_utils.h"

// Read training data
void readTraindata(const string dataRoot, const string sequenceName,
                   vector<string> &color_list, vector<string>& depth_list,
                   float* &extrinsic, int* numofframe, Camera &cam_K);

// Class to hold training data
class DataTrain {
public:
  vector<string> color_list;    /**< list of paths to color images*/
  vector<string> depth_list;    /**< list of paths to depth images */
  float* extrinsic;
  int numofframe;
  Camera camera;
  DataTrain(const string, const string);
};

DataTrain::DataTrain(const string dataRoot, const string sequenceName) {
  readTraindata(dataRoot, sequenceName, color_list, depth_list, extrinsic, &numofframe, camera);
}

/**
 * readTraindata: Get paths for images and load initial data. Expects
 * files called 'colorTrain.txt' and 'depthTrain.txt' in the directory
 * dataRoot/sequenceName. These files should contain the number of
 * frames in the first line followed by the paths to images, one on
 * each line.
 *
 * @param dataRoot
 * @param sequenceName
 * @param color_list
 * @param depth_list
 * @param extrinsic
 * @param numofframe
 * @param cam_K
 */
void readTraindata(const string dataRoot, const string sequenceName,
                   vector<string> &color_list, vector<string>& depth_list,
                   float* &extrinsic, int* numofframe, Camera &cam_K) {

  /// Get list of paths to color images
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


  /// Get list of paths to depth images
  string listfile_depth = dataRoot + sequenceName + "depthTrain.txt";
  ifstream file_depth(listfile_depth);
  if (file_depth.is_open()) {
    getline(file_depth, line);
    while (getline(file_depth, line)) {
      depth_list.push_back(line);
    }
    file_depth.close();
  }
  else cout << "Unable to open file: " << listfile_depth;

  cout<< "numofframe : " << numofframe[0] << endl;
  cout<< "color List : " << color_list.size() << endl;
  cout<< "depth List : " << depth_list.size() << endl;

  /// Get extrinsic matrices of color camera because color and depth
  /// are already aligned
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

  /// Get camera intrinsics
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
    //cout << cam_K.fx << "," << cam_K.fy << "," << cam_K.cx<<"," << cam_K.cy << endl;
  }
  else {
    cout << "Unable to open file: " << cam_file;
  }

  return;
}
