#include "Parameters.h"

Parameters::Parameters(const string dataRoot, const string sequenceName) {
  readFromFile(dataRoot, sequenceName);
}

Parameters::~Parameters() {
  free(extrinsic);
  free(projection_d2c);
}

void Parameters::readFromFile(const string dataRoot, const string sequenceName) {
  cout << dataRoot << " " << sequenceName << endl;

  /// Get list of paths to color images
  string listfile_color = dataRoot + sequenceName + "colorTrain.txt";
  string line;
  ifstream file_color (listfile_color);
  if (file_color.is_open()) {
    getline(file_color, line);
    numofframe = atoi( line.c_str() );
    while (getline(file_color, line)) {
      color_list.push_back(line);
    }
    file_color.close();
  }
  else cout << "Unable to open file (color): " << listfile_color << endl;

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
  else cout << "Unable to open file (depth): " << listfile_depth << endl;;

  cout<< "numofframe : " << numofframe << endl;
  cout<< "color List : " << color_list.size() << endl;
  cout<< "depth List : " << depth_list.size() << endl;

  // Get extrinsic matrix of color camera
  string extrinsic_file = dataRoot + sequenceName + "extrinsics.txt";
  ifstream ex_myfile(extrinsic_file);
  if (ex_myfile.is_open()) {
    extrinsic = (float*) malloc (sizeof(float) * numofframe * 12);
    //int result = fread (extrinsic,sizeof(float),numofframe*12,pFile);
    //fclose(pFile);
    for (int i = 0; i < 12 * numofframe; i++) {
      ex_myfile >> extrinsic[i];
    }
  }

  /// Get color camera intrinsics
  string color_intrinsics_file_path = dataRoot + sequenceName + "intrinsics.txt";
  ifstream  color_intrinsics_file(color_intrinsics_file_path);
  float tmp;
  if (color_intrinsics_file.is_open()) {
    color_intrinsics_file >> color_camera.fx;
    color_intrinsics_file >> tmp;
    color_intrinsics_file >> color_camera.cx;
    color_intrinsics_file >> tmp;
    color_intrinsics_file >> color_camera.fy;
    color_intrinsics_file >> color_camera.cy;
    color_intrinsics_file.close();
  }
  else {
    cout << "Unable to open file (color intrinsics): " << color_intrinsics_file;
  }

  /// Get depth camera intrinsics and depth extrinsic matrix w.r.t. color
  string depth_intrinsics_file_path = dataRoot + sequenceName + "intrinsics_d2c.txt";
  ifstream depth_intrinsics_file(depth_intrinsics_file_path);

  if (depth_intrinsics_file.is_open()) {
    // Grab depth intrinsics data
    depth_intrinsics_file >> depth_camera.fx;
    depth_intrinsics_file >> tmp;
    depth_intrinsics_file >> depth_camera.cx;
    depth_intrinsics_file >> tmp;
    depth_intrinsics_file >> depth_camera.fy;
    depth_intrinsics_file >> depth_camera.cy;
    depth_intrinsics_file >> tmp;
    depth_intrinsics_file >> tmp;
    depth_intrinsics_file >> tmp;

    // Grab projection from depth to color
    projection_d2c = (float *)malloc(sizeof(float) * 12);
    for (int i = 0; i < 12; i++) {
      depth_intrinsics_file >> projection_d2c[i];
    }

    depth_intrinsics_file.close();

    cout << depth_camera.fx << ", " << depth_camera.cx << ", " << depth_camera.fy << ", " << depth_camera.cy << endl;

    for (int i = 0; i < 12; i++) {
      cout << projection_d2c[i];
      if ((i + 1) % 4 == 0) {
        cout << endl;
      }
    }
  }

  return;
}
