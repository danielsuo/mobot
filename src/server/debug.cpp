#include "debug.h"

vector<int> getIndicesC() {

  int NUM_FRAMES = 284;
  // First, use MATLAB centers
  cuBoF *bag = new cuBoF("../result/kdtree/centers.bof");

/*
  float total = 0;
  for (int i = 0; i < bag->numFeatures; i++) {
    float sum = 0;
    for (int j = 0; j < bag->numDimensions; j++) {
      sum += bag->features[i * bag->numDimensions + j];
    }
    cerr << "Sum for feature " << i << " " << sum << endl;
    total += sum;
  }

  cerr << "Sum for total " << total << endl;
*/
  vector<float *> histograms;
  vector<int> indices;
  float sqrt2 = sqrt(2);

  
  cerr << "Computing histograms" << endl;
  float total = 0;
  for (int i = 0; i < NUM_FRAMES; i++) {
    // float *histogram = bag->vectorize(&frames[i]->pairs[0]->siftData);

    SiftData *data = new SiftData();
    string path = "../result/sift/sift" + to_string(i + 1);
    ReadVLFeatSiftData(*data, path.c_str());

    // float siftsum = 0;
    // for (int j = 0; j < data->numPts; j++) {
    //   for (int k = 0; k < 128; k++) {
    //     siftsum += data->h_data[j].data[k];
    //     if (i == 0 && j == 0) {
    //       fprintf(stderr, "%0.10f\n", data->h_data[j].data[k]);
    //     }
    //   }
    // }
    // cerr << "Sift sum for frame " << i << " " << siftsum << endl;

    float *histogram = bag->vectorize(data);

    float sum = 0;
    for (int j = 0; j < bag->numFeatures; j++) {
      sum += histogram[j];
    }
    // cerr << "Sum for frame " << i << " " << sum << endl;

    total += sum;

    histograms.push_back(histogram);
    indices.push_back(2 * i);
    indices.push_back(2 * i + 1);
  }

  cerr << "Sum for total " << total << endl;

  for (int i = 0; i < NUM_FRAMES; i++) {
    // cerr << "Processing frame " << i << " of " << NUM_FRAMES << endl;

    vector<float> weights(NUM_FRAMES, 0);
    for (int j = 0; j < NUM_FRAMES; j++) {
      // Calculate as bwdist from Matlab would
      int index = abs(i - j);
      float weight = index / 2;
      if (index % 2 == 0) {
        weight = weight * sqrt2;
      } else {
        weight = sqrt(weight * weight + (weight + 1) * (weight + 1));
      }

      // Divide by 150 because magic numbers!! (Replicating matlab result)
      if (i < j) weights[j] = min(1.0, weight / 150.0);
    }

    // cerr << endl;

    float maxWeightedScore = 0;
    float maxOrigScore = 0;
    int maxIndex = -1;

    vector<float> scores;
    vector<float> weightedScores;

    ofstream myfile;
    myfile.open("../result/matches/scores_" + to_string(i) + ".csv");
    for (int j = 0; j < histograms.size(); j++) {
      float origScore = dot(histograms[i], histograms[j], bag->numFeatures);
      float weightedScore = origScore * weights[j];
      if (weightedScore > maxWeightedScore) {
      // if (origScore > maxOrigScore) {
        maxOrigScore = origScore;
        maxWeightedScore = weightedScore;
        maxIndex = j;
      }
      scores.push_back(origScore);
      weightedScores.push_back(weightedScore);
      myfile << origScore << "," << weightedScore << "," << weights[j] << endl;
    }
    myfile.close();

    if (maxWeightedScore > 0.2) {
      // cerr << "Pair found (" << maxOrigScore << ", " << maxWeightedScore << ") at (" << i << ", " << maxIndex << ") with weight " << weights[maxIndex] << endl; 
      // match1.push_back(i);
      // match2.push_back(maxIndex);
      indices.push_back(i);
      indices.push_back(maxIndex);
    }
  }

  // Free histogram data
  for (int i = 0; i < NUM_FRAMES; i++) {
    free(histograms[i]);
  }

  return indices;
}

void compareIndices() {
  vector<int> indices = ReadMATLABIndices("../result/indices.bin");
  vector<int> indicesC = getIndicesC();

  for (int i = 0; i < indices.size() / 2; i++) {
    int frame1 = indices[2 * i] - 1;
    int frame2 = indices[2 * i + 1] - 1;

    // for 
  }
}

void testReadFloatSift() {
  for (int i = 0; i < 284; i++) {
    string path = "../result/sift/sift" + to_string(i + 1);
    vector<float *> result = ReadVLFeatSiftDataAsFloatArray(path.c_str());

    float sum = 0;
    for (int j = 0; j < result.size(); j++) {
      for (int k = 0; k < 128; k++) {
        sum += result[j][k];
      }
    }
    cerr << "Checksum: " << sum << endl;
  }
}

void testReadIndicesFromMatlab() {
  ReadMATLABIndices("../result/indices.bin");
}

void testReadArrayFromMatlab() {
  double Rt[12];
  ReadMATLABRt(Rt, "../result/testMajor.bin");
}

void testCeresRotationMatrix() {
  float t[12] = {1.0000f, -0.0002f, -0.0014f, 0.0035f,
    0.0002f, 1.0000f, -0.0007f, 0.0016f,
    0.0014f, 0.0007f, 1.0000f, -0.0008f};

    double *a = new double[6];

    float *r = new float[12];

    TransformationMatrixToAngleAxisAndTranslation(t, a);
    AngleAxisAndTranslationToTransformationMatrix(a, r);

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 4; j++) {
        cerr << r[i * 4 + j] << " ";
      }
      cerr << endl;
    }

    double pt[3] = {1, 10, 20};
    double pt2[3];
    AngleAxisRotateAndTranslatePoint(a, pt, pt);
    for (int i = 0; i < 3; i++) {
      cerr << pt[i] << " ";
    }
    cerr << endl;

    AngleAxisRotateAndTranslatePoint(a, pt, pt, false);

    for (int i = 0; i < 3; i++) {
      cerr << pt[i] << " ";
    }
    cerr << endl;
  }

  void testRandomIntVector() {
    vector<int> test = getRandomIntVector(1, 100, 100);

    for (int i = 0; i < 100; i++) {
      cerr << i << ": " << test[i] << endl;
    }
  }

  void testCUBOF() {
    cuBoF *bag = new cuBoF("matlab.bof");

    delete bag;
  }

  void testCUBOFCreate() {
    vector<SiftData *> siftData;
    int totalNumSIFT = 0;

    for (int i = 0; i < 284; i++) {
      SiftData *data = new SiftData();
      siftData.push_back(data);
    }

    for (int i = 0; i < 284; i++) {
      string path = "../result/sift/sift" + to_string(i + 1);
      ReadVLFeatSiftData(*siftData[i], path.c_str());
      cerr << "Num sift points read " << siftData[i]->numPts << endl;
      totalNumSIFT += siftData[i]->numPts;
    }

    cuBoF *bag = new cuBoF(4000, 284);
    bag->train(siftData, totalNumSIFT);
    bag->save("matlab.bof");
    delete bag;
  }

  void testCerberus() {
    Cerberus *solver = new Cerberus();
    solver->solve();
  }

  void testSift() {
    string device1_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg";
    string device1_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png";
    Pair *pair = new Pair(device1_color, device1_depth);

  // for (int i = 0; i < pair->siftData.numPts; i++) {
  //   for (int j = 0; j < 128; j++) {
  //     fprintf(stderr, "%0.1f ", pair->siftData.h_data[i].data[j]);
  //   }
  //   fprintf(stderr,"\n");
  // }
  }

  void testMatching() {
    string pair1_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg";
    string pair1_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png";
    string pair2_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.50.113-0000000205.jpg";
    string pair2_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.50.113-0000000205.png";

    Pair *device1 = new Pair(pair1_color, pair1_depth);
    Pair *device2 = new Pair(pair2_color, pair2_depth);

    cv::Mat curr_match(0, 3, cv::DataType<float>::type);
    cv::Mat next_match(0, 3, cv::DataType<float>::type);

  // int numMatchedPoints = device1->getMatched3DPoints(device2, curr_match, next_match);
  // fprintf(stderr, "Number filtered matched points: %d\n", numMatchedPoints);
  }

  void testMatchingNearlySameImage() {
    string pair1_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg";
    string pair1_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png";
    string pair2_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.381-0000000164.jpg";
    string pair2_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.381-0000000164.png";

    Pair *pair11 = new Pair(pair1_color, pair1_depth);
    Pair *pair12 = new Pair(pair2_color, pair2_depth);
    Pair *pair21 = new Pair(pair1_color, pair1_depth);
    Pair *pair22 = new Pair(pair2_color, pair2_depth);

    MatchSiftData(pair11->siftData, pair12->siftData, MatchSiftDistanceL2);
    MatchSiftData(pair21->siftData, pair22->siftData, MatchSiftDistanceDotProduct);

    SiftPoint *siftPoints1 = pair11->siftData.h_data;
    SiftPoint *siftPoints2 = pair21->siftData.h_data;

    for (int i = 0; i < pair11->siftData.numPts; i++) {

      fprintf(stderr, "Matches: L2 (%0.4f, %0.4f) <> (%0.4f, %0.4f), DP (%0.4f, %0.4f) <> (%0.4f, %0.4f)\n", 
        siftPoints1[i].coords2D[0], siftPoints1[i].coords2D[1], siftPoints1[i].match_xpos, siftPoints1[i].match_ypos,
        siftPoints2[i].coords2D[0], siftPoints2[i].coords2D[1], siftPoints2[i].match_xpos, siftPoints2[i].match_ypos);
    }

    Pair *device1 = new Pair(pair1_color, pair1_depth);
    Pair *device2 = new Pair(pair2_color, pair2_depth);

    cv::Mat curr_match(0, 3, cv::DataType<float>::type);
    cv::Mat next_match(0, 3, cv::DataType<float>::type);

  // int numMatchedPoints = device1->getMatched3DPoints(device2, curr_match, next_match);
  // fprintf(stderr, "Number filtered matched points: %d\n", numMatchedPoints);
  }

  void readDataFromBlobToDisk() {
    DeviceManager *manager = new DeviceManager();
    manager->addDeviceByFilePath((char *)"device1", (char *)"device1", ParserOutputModeDisk);
    manager->addDeviceByFilePath((char *)"device2", (char *)"device2", ParserOutputModeDisk);
    manager->addDeviceByFilePath((char *)"device3", (char *)"device3", ParserOutputModeDisk);
    manager->addDeviceByFilePath((char *)"device4", (char *)"device4", ParserOutputModeDisk);

    manager->digest();
  }

  void readDataFromTCPToMemory() {
    TCPServer *server = new TCPServer(8125);
    DeviceManager *manager = server->manager;

  // manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124);
  // manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124);
    manager->addDeviceByStringIPAddress((char *)"iPhone", (char *)"192.168.0.109", 8124, ParserOutputModeMemory);

    server->connect();
    server->listen();
    manager->runLoop();

    pthread_exit(NULL);
  }

  void readDataFromTCPToBlob() {
    TCPServer *server = new TCPServer(8124);
    DeviceManager *manager = server->manager;
    // manager->strategy = Strategy::createStrategy(DefaultStrategyType);

    manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124, ParserOutputModeBlob);
    manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124, ParserOutputModeBlob);
    manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124, ParserOutputModeBlob);
    manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124, ParserOutputModeBlob);
    // manager->addDeviceByStringIPAddress((char *)"iPhone", (char *)"192.168.0.109", 8124, ParserOutputModeMemory);

    server->connect();
    server->listen();
    // manager->runLoop();

    pthread_exit(NULL);
  }

  void readDataFromBlobToMemory() {
    json config;

    ifstream configFile;
    configFile.open("../config/main.json");
    configFile >> config;
    configFile.close();

  // cout << config.dump(4) << endl;

    DeviceManager *manager = new DeviceManager();

    for (int i = 0; i < NUM_DEVICES; i++) {

      string name = config["devices"][i]["name"];
      string path = config["devices"][i]["path"];
      manager->addDeviceByFilePath((char *)name.c_str(), (char *)path.c_str(), ParserOutputModeMemory);

      vector<float> extrinsic = config["devices"][i]["extrinsicMatrixRelativeToFirstCamera"];
      manager->devices[i]->extrinsicMatrixRelativeToFirstCamera = new float[12];

      for (int j = 0; j < 12; j++) {
        manager->devices[i]->extrinsicMatrixRelativeToFirstCamera[j] = extrinsic[j];
      }

      manager->devices[i]->scaleRelativeToFirstCamera = config["devices"][i]["scaleRelativeToFirstCamera"];
    }

    manager->digest();

    for (int i = 0; i < NUM_DEVICES; i++) {
      string timestamp_path = config["devices"][i]["timestamp_path"];
      manager->devices[i]->readTimestamps((char *)timestamp_path.c_str());
    }

    manager->runLoop();

    delete manager;
  }

  void collectWheelToHeadCalibrationDataFromRobot() {
    TCPServer *server = new TCPServer(8124);
    DeviceManager *manager = server->manager;

    manager->addDeviceByStringIPAddress((char *)"device1", (char *)"192.168.0.108", 8124, ParserOutputModeBlob);
    manager->addDeviceByStringIPAddress((char *)"device2", (char *)"192.168.0.106", 8124, ParserOutputModeBlob);
    manager->addDeviceByStringIPAddress((char *)"device3", (char *)"192.168.0.105", 8124, ParserOutputModeBlob);
    manager->addDeviceByStringIPAddress((char *)"device4", (char *)"192.168.0.107", 8124, ParserOutputModeBlob);

    server->connect();
    server->listen();

    Mobot mobot((char *)"192.168.0.129", 8125);
    mobot.connect();
    mobot.listen();

    sleep(5);

    mobot.turn(-360);

    while (mobot.progress < 100) {
      fprintf(stderr, "Current progress: %d\n", mobot.progress);
      sleep(1);
    }

    server->disconnect();
    mobot.disconnect();
  }

  void testOccupancyGridCalculation() {
    string device1_color = "image_data/device1/2016-01-11T10.10.44.340/color/2016-01-11T10.10.47.319-0000000163.jpg";
    string device1_depth = "image_data/device1/2016-01-11T10.10.44.340/depth/2016-01-11T10.10.47.319-0000000163.png";
    string device2_color = "image_data/device2/2016-01-11T10.11.28.502/color/2016-01-11T10.11.31.475-0000000177.jpg";
    string device2_depth = "image_data/device2/2016-01-11T10.11.28.502/depth/2016-01-11T10.11.31.475-0000000177.png";
    string device3_color = "image_data/device3/2016-01-11T10.10.44.791/color/2016-01-11T10.10.47.760-0000000191.jpg";
    string device3_depth = "image_data/device3/2016-01-11T10.10.44.791/depth/2016-01-11T10.10.47.760-0000000191.png";
    string device4_color = "image_data/device4/2016-01-11T10.10.44.358/color/2016-01-11T10.10.47.326-0000000133.jpg";
    string device4_depth = "image_data/device4/2016-01-11T10.10.44.358/depth/2016-01-11T10.10.47.326-0000000133.png";

    Pair *device1 = new Pair(device1_color, device1_depth);
    Pair *device2 = new Pair(device2_color, device2_depth);
    Pair *device3 = new Pair(device3_color, device3_depth);
    Pair *device4 = new Pair(device4_color, device4_depth);

    Frame *frame = new Frame(4);
    frame->pairs[0] = device1;
    frame->pairs[1] = device2;
    frame->pairs[2] = device3;
    frame->pairs[3] = device4;

    fprintf(stderr, "Num pairs %lu\n", frame->pairs.size());

    json config;

    ifstream configFile;
    configFile.open("../config/main.json");
    configFile >> config;
    configFile.close();

    cout << config.dump(4) << endl;

    for (int i = 0; i < 4; i++) {
      float tmp[12];
      for (int j = 0; j < 12; j++) {
        tmp[j] = config["devices"][i]["extrinsicMatrixRelativeToFirstCamera"][j];
        fprintf(stderr, "%0.4f ", tmp[j]);
      }
      fprintf(stderr, "\n");

      fprintf(stderr, "Point cloud size: %d\n", frame->pairs[i]->pointCloud->color.size().height *  frame->pairs[i]->pointCloud->color.size().width);
      frame->buildPointCloud(i, 1, tmp);
    }

    frame->pointCloud_world->copy(frame->pointCloud_camera);
    frame->writePointCloud();

    float minx, maxx, miny, maxy, minz, maxz;
    frame->pointCloud_world->getExtents(minx, maxx, miny, maxy, minz, maxz);

    fprintf(stderr, "Bounds x: (%0.4f, %0.4f), y: (%0.4f, %0.4f), z: (%0.4f, %0.4f)\n", minx, maxx, miny, maxy, minz, maxz);
  }

  int testBundleAdjustment(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);

  // NOTE: Data from MATLAB is in column-major order

    cout << "Ba2D3D bundle adjuster in 2D and 3D. Writen by Jianxiong Xiao." << endl;
    cout << "Usage: EXE mode(1,2,3,5) w3Dv2D input_file_name output_file_name" << endl;

  // Get bundle adjustment mode
    int mode = atoi(argv[1]);

  // TODO: figure out
    int NUM_BA_POINTS = atof(argv[2]);

  // Open input file
    FILE* fp = fopen(argv[3],"rb");
    if (fp == NULL) {
      cout << "fail to open file" << endl;
      return false;
    }

  // Get number of camera poses (i.e., frames; assumes that each pose
  // uses the same camera)
    uint32_t nCam;  fread((void*)(&nCam), sizeof(uint32_t), 1, fp);

  // Get number of matched pairs
    uint32_t nPairs; fread((void*)(&nPairs), sizeof(uint32_t), 1, fp);

  // finish reading
    fclose(fp);

  // Read all of the extrinsic matrices for the nCam camera
  // poses. Converts camera coordinates into world coordinates
    double* cameraRtC2W = new double [12*nCam];

  // Construct camera parameters from camera matrix
    double *cameraParameter = new double [6*nCam];
    double* cameraParameter_ij = new double[6*nPairs];

    Cerberus *c1 = new Cerberus();
    Cerberus *c2 = new Cerberus();

  // Which matched point are we on?
    uint32_t points_count = 0;
    double *points_observed_i = new double[3 * NUM_BA_POINTS * nPairs];
    double *points_observed_j = new double[3 * NUM_BA_POINTS * nPairs];
    double *points_predicted = new double[3 * NUM_BA_POINTS * nPairs];
    vector<int> indices = ReadMATLABIndices("../../server/result/indices.bin");

    cout << "Building problem" << endl;

  // TODO: compute inverse outside
    for (int i = 0; i < nPairs; i++) {

    // Get initial values for Rt_i and Rt_j. Note that we subtract one
    // from our offset because MATLAB is 1-indexed. Note that these
    // transforms are camera to world coordinates
      double *Rt_i = cameraParameter + 6 * (indices[2 * i]);
      double *Rt_j = cameraParameter + 6 * (indices[2 * i + 1]);

      double tmp[12];
      string path = "../../server/result/Rt/Rt" + to_string(indices[2 * i] + 1);
      ReadMATLABRt(tmp, path.c_str());
      TransformationMatrixToAngleAxisAndTranslation(tmp, Rt_i);
      path = "../../server/result/Rt/Rt" + to_string(indices[2 * i + 1] + 1);
      ReadMATLABRt(tmp, path.c_str());
      TransformationMatrixToAngleAxisAndTranslation(tmp, Rt_j);

    // Get relative pose between matched frames
      double *Rt_ij = cameraParameter_ij + 6 * i;
      path = "../../server/result/Rt/Rt" + to_string(indices[2 * i] + 1) + "_" + to_string(indices[2 * i + 1] + 1);
      ReadMATLABRt(tmp, path.c_str());
      TransformationMatrixToAngleAxisAndTranslation(tmp, Rt_ij);

    // Overweight time-based alignment
      double w = indices[2 * i + 1] - indices[2 * i] == 1 ? 50.0 : 1.0;
      double weight_PoseGraph[9] = {w, w, w, w, w, w, w, w, w};

      PoseGraphResidual::AddResidualBlock(c1, Rt_ij, weight_PoseGraph, Rt_i, Rt_j);

    // Overweight loop closures
      w = indices[2 * i + 1] - indices[2 * i] == 1 ? 1 : 1;
      double weight_BundleAdjustment[3] = {w, w, w};
      string matchesPath = "../../server/result/match/match" + to_string(indices[2 * i] + 1) + "_" + to_string(indices[2 * i + 1] + 1);
      vector<SiftMatch *> matches = ReadMATLABMatchData(matchesPath.c_str());

      int num_points = min(NUM_BA_POINTS, (int)matches.size());

      for (int j = 0; j < num_points; j++) {
        int index = (points_count + j) * 3;

        double *point_observed_i = points_observed_i + index;
        double *point_observed_j = points_observed_j + index;
        double *point_predicted = points_predicted + index;

        point_observed_i[0] = matches[j]->pt1->coords3D[0];
        point_observed_i[1] = matches[j]->pt1->coords3D[1];
        point_observed_i[2] = matches[j]->pt1->coords3D[2];
        point_observed_j[0] = matches[j]->pt2->coords3D[0];
        point_observed_j[1] = matches[j]->pt2->coords3D[1];
        point_observed_j[2] = matches[j]->pt2->coords3D[2];

        AngleAxisRotateAndTranslatePoint(Rt_i, point_observed_i, point_predicted);

      // TODO: replace these with pointer to Cerberus
        BundleAdjustmentResidual::AddResidualBlock(c2, point_observed_i, weight_BundleAdjustment, Rt_i, point_predicted);
        BundleAdjustmentResidual::AddResidualBlock(c2, point_observed_j, weight_BundleAdjustment, Rt_j, point_predicted);
      }

      points_count += num_points;
    }

  //----------------------------------------------------------------

  // cout << "Starting full pose graph solver" << endl;
    c1->solve();

  // cout << "Starting pose graph BA solver" << endl;
  // c2->solve();

  // obtain camera matrix from parameters
    for(int cameraID = 0; cameraID < nCam; cameraID++){
      double* cameraPtr = cameraParameter + 6 * cameraID;
      double* cameraMat = cameraRtC2W + 12 * cameraID;
      AngleAxisAndTranslationToTransformationMatrix(cameraPtr, cameraMat, true);
    }

  // write back result files
    FILE* fpout = fopen(argv[4],"wb");
    fwrite((void*)(cameraRtC2W), sizeof(double), 12*nCam, fpout);

    fclose (fpout);

  // clean up
    delete [] cameraRtC2W;
    delete [] cameraParameter;
    delete [] cameraParameter_ij;
    delete [] points_observed_i;
    delete [] points_observed_j;
    delete [] points_predicted;

    return 0;
  }

  void buildGrid() {
    Grid *grid = new Grid(11, 11);

    for (int j = 4; j < 10; j++) {
      grid->setOccupied(3, j, true);
    }

    grid->print();

  // for (int i = -10; i < 10; i++) {
  //   for (int j = -10; j < 10; j++) {
  //     if (grid->inBounds(i, j)) {
  //       cerr << "Herro!" << i << " " << j << endl;
  //     }
  //   }
  // }
    grid->resize(GridSideLeft, 3);
    grid->print();
    grid->resize(GridSideUp, 2);
    grid->print();
    grid->resize(GridSideRight, 3);
    grid->print();
    grid->resize(GridSideDown, 3);
    grid->print();
    grid->resize(GridSideDown, -3);
    grid->print();


    grid->shift(1, 1);
    grid->print();
    grid->shift(-1, -1);
    grid->print();
    grid->shift(5, 0);
    grid->print();
    grid->move(1, 1);
    grid->print();
    grid->move(-1, -1);
    grid->print();

  // grid->getNextPath();

    Grid *grid2 = new Grid(10, 10, 5, 5);
    grid2->print();

  // grid2->shift(10, 10);

  // for (int i = 0; i < grid2->width - 2; i++) {
  //   for (int j = 0; j < grid2->height - 2; j++) {
  //     if ((i + j) % 2 == 0) grid2->setOccupied(i, j, true);
  //   }
  // }
  // vector<GridPoint *> path = grid2->getNextPath();
  // cerr << path.size() << endl;
  // grid->integrate(grid2);

  // Grid *grid3 = new Grid(5, 5);
  // grid3->shift(-1, -4);
  // for (int i = 0; i < grid3->width; i++) {
  //   for (int j = 0; j < grid3->height; j++) {
  //     grid3->setOccupied(i, j, true);
  //   }
  // }
  // grid->integrate(grid3);
  }

void testRounding() {
  float test = 0.2;
  float test2 = -0.2;
  int resolution = 4;

  cerr << test / resolution << endl;
  cerr << test2 / resolution << endl;

  cerr << round(test / resolution) << endl;
  cerr << round(test2 / resolution) << endl;

  int itest = round(test / resolution);
  int itest2 = round(test2 / resolution);

  cerr << itest << endl;
  cerr << itest2 << endl;
}

void testMobot() {
  Mobot mobot((char *)"192.168.0.129", 8125);
  mobot.connect();
  mobot.listen();

  sleep(5);

  mobot.navigate(0, 0, 0, -1);

  // mobot.turn(-360);

  // while (mobot.progress < 100) {
  //   fprintf(stderr, "Current progress: %d\n", mobot.progress);
  //   sleep(1);
  // }

  // mobot.turn(360);

  // while (mobot.progress < 100) {
  //   fprintf(stderr, "Current progress: %d\n", mobot.progress);
  //   sleep(1);
  // }

  // mobot.turn(180);

  // while (mobot.progress < 100) {
  //   fprintf(stderr, "Current progress: %d\n", mobot.progress);
  //   sleep(1);
  // }

  // mobot.drive(1000);

  // while (mobot.progress < 100) {
  //   fprintf(stderr, "Current progress: %d\n", mobot.progress);
  //   sleep(1);
  // }

  mobot.disconnect();
}

void initializeMobot() {
  Mobot mobot((char *)"192.168.0.129", 8125);
  mobot.connect();
  mobot.listen();

  sleep(5);

  mobot.rotate(360);

  mobot.turn(45);
  mobot.turn(-45);
}

void driveRotate(Mobot &mobot, int amount) {
  int steps = amount / 250;

  for (int i = 0; i < steps; i++) {
    mobot.drive(250);
    mobot.rotate(360);
  }
}

void *mobotDemo(void *pointer) {
  Mobot mobot((char *)"192.168.0.129", 8125);
  mobot.connect();
  mobot.listen();

  sleep(2);

  // mobot.turn(180);
  driveRotate(mobot, 1750);
  mobot.turn(-90);
  driveRotate(mobot, 2500);

  mobot.turn(-90);
  driveRotate(mobot, 250);
  mobot.turn(-90);
  driveRotate(mobot, 3000);
  mobot.turn(90);
  driveRotate(mobot, 250);
  mobot.turn(90);
  driveRotate(mobot, 3000);

  mobot.turn(-90);
  driveRotate(mobot, 250);
  mobot.turn(-90);
  driveRotate(mobot, 3000);
  mobot.turn(90);
  driveRotate(mobot, 250);
  mobot.turn(90);
  driveRotate(mobot, 3000);

  mobot.turn(-180);
  // driveRotate(mobot, 250);
  // mobot.turn(-90);
  driveRotate(mobot, 3000);

  mobot.disconnect();
}