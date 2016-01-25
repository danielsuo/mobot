#include "ReplicateMatlabStrategy.h"

ReplicateMatlabStrategy::ReplicateMatlabStrategy() : Strategy() {
  // Initialize two solvers; one for pose graph, one for bundle adjustment
  solvers.push_back(new Cerberus());
  solvers.push_back(new Cerberus());
}

ReplicateMatlabStrategy::~ReplicateMatlabStrategy() {
  delete bag;
  for (int i = 0; i < solvers.size(); i++) {
    delete solvers[i];
  }
}

template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

  // initialize original index locations
  vector<size_t> idx(v.size());
  for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}

// void getAngleAxis(int i, int j, float *result) {
//   float tmp[12];
//   string path = "../result/Rt/Rt" + to_string(i) + "_" + to_string(j);
//   ReadMATLABRt(tmp, path.c_str());

//   TransformationMatrixToAngleAxisAndTranslation(tmp, result);
// }

void getMATLABRt(double *Rt, int i, int j = -1) {
  double tmp[12];
  string path = "../result/Rt/Rt" + to_string(i + 1);
  if (j > -1) {
    path += "_" + to_string(j + 1);
  }

  ReadMATLABRt(tmp, path.c_str());
  TransformationMatrixToAngleAxisAndTranslation(tmp, Rt);
}

vector<SiftMatch *> getMATLABMatches(int i, int j) {
  string path = "../result/match/match" + to_string(i + 1) + "_" + to_string(j + 1);
  return ReadMATLABMatchData(path.c_str());
}

void ReplicateMatlabStrategy::getRt(double *Rt, int index) {
  TransformationMatrixToAngleAxisAndTranslation(frames[index]->Rt_absolute, Rt);
}

vector<SiftMatch *> ReplicateMatlabStrategy::getMatches(int i, int j, double *Rt) {
  vector<SiftMatch *> matches;
  float tmp[12];

  // getMATLABRt(Rt, i, j);
  // matches = getMATLABMatches(i, j);
  // return matches;
    
  if (i - j == 1 || j - i == 1) {
    memcpy(tmp, frames[min(i, j)]->Rt_relative, sizeof(float) * 12);
    matches = frames[min(i, j)]->matches;
  } else {
    matches = frames[i]->computeRelativeTransform(frames[j], tmp);
  }

  TransformationMatrixToAngleAxisAndTranslation(tmp, Rt);
  return matches;
}

vector<int> getIndices() {
  return ReadMATLABIndices("../result/indices.bin");
}

vector<int> ReplicateMatlabStrategy::getFramePairs() {
  // First, use MATLAB centers
  // bag = new cuBoF("../result/kdtree/centers.bof");
  bag = new cuBoF("tmp.bof");

  vector<float *> histograms;
  vector<int> indices;
  float sqrt2 = sqrt(2);

  cerr << "Computing histograms" << endl;
  for (int i = 0; i < frames.size(); i++) {
    float *histogram = bag->vectorize(&frames[i]->pairs[0]->siftData);

    // SiftData *data = new SiftData();
    // string path = "../result/sift/sift" + to_string(i + 1);
    // ReadVLFeatSiftData(*data, path.c_str());
    // float *histogram = bag->vectorize(data);

    histograms.push_back(histogram);
  }

  for (int i = 0; i < frames.size() - 1; i++) {
    indices.push_back(i);
    indices.push_back(i + 1);
  }

  for (int i = 0; i < frames.size(); i++) {
    // cerr << "Processing frame " << i << " of " << frames.size() << endl;

    vector<float> weights(frames.size(), 0);
    for (int j = 0; j < frames.size(); j++) {
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
      cerr << "Pair found (" << maxOrigScore << ", " << maxWeightedScore << ") at (" << i << ", " << maxIndex << ") with weight " << weights[maxIndex] << endl; 
      // match1.push_back(i);
      // match2.push_back(maxIndex);
      indices.push_back(i);
      indices.push_back(maxIndex);
    }
  }

  // Free histogram data
  for (int i = 0; i < frames.size(); i++) {
    free(histograms[i]);
  }

  return indices;
}

bool MATLAB = false;

void ReplicateMatlabStrategy::processLastFrame() {
  vector<int> indices;
  if (MATLAB) {
    indices = getIndices();
  } else {
    indices = getFramePairs();
  }

  int nCam = frames.size();
  int nPairs = indices.size() / 2;

  // Read all of the extrinsic matrices for the nCam camera
  // poses. Converts camera coordinates into world coordinates
  double* cameraRtC2W = new double [12 * nCam];

  // Construct camera parameters from camera matrix
  double *cameraParameter = new double [6 * nCam];
  double* cameraParameter_ij = new double[6 * nPairs];

  uint32_t points_count = 0;
  double *points_observed_i = new double[3 * 500 * nPairs];
  double *points_observed_j = new double[3 * 500 * nPairs];
  double *points_predicted = new double[3 * 500 * nPairs];
  // double w = indices[2 * i + 1] - indices[2 * i] == 1 ? 50.0 : 1.0;
  double w = 1;
  double weight_PoseGraph[9] = {w, w, w, w, w, w, w, w, w};
  double weight_BundleAdjustment[3] = {w, w, w};

  for (int i = 0; i < nPairs; i++) {
    double *Rt_ij = cameraParameter_ij + 6 * i;
    double *Rt_i = cameraParameter + 6 * (indices[2 * i]);
    double *Rt_j = cameraParameter + 6 * (indices[2 * i + 1]);

    vector<SiftMatch *> matches;
    if (MATLAB) {
      getMATLABRt(Rt_ij, indices[2 * i], indices[2 * i + 1]);
      getMATLABRt(Rt_i, indices[2 * i]);
      getMATLABRt(Rt_j, indices[2 * i + 1]);      
    } else {
      matches = getMatches(indices[2 * i], indices[2 * i + 1], Rt_ij);
      getRt(Rt_i, indices[2 * i]);
      getRt(Rt_j, indices[2 * i + 1]);
    }

    PoseGraphResidual::AddResidualBlock(solvers[0], Rt_ij, weight_PoseGraph, Rt_i, Rt_j);

    int num_points = min(500, (int)matches.size());

    // fprintf(stderr, "Points for pairs (%d, %d)\n", indices[2 * i], indices[2 * i + 1]);
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

      // fprintf(stderr, "%0.4f %0.4f %0.4f\n", point_observed_i[0], point_observed_i[1], point_observed_i[2]);
      // fprintf(stderr, "%0.4f %0.4f %0.4f\n", point_predicted[0], point_predicted[1], point_predicted[2]);
      // fprintf(stderr, "\n");

      // TODO: replace these with pointer to Cerberus
      BundleAdjustmentResidual::AddResidualBlock(solvers[1], point_observed_i, weight_BundleAdjustment, Rt_i, point_predicted);
      BundleAdjustmentResidual::AddResidualBlock(solvers[1], point_observed_j, weight_BundleAdjustment, Rt_j, point_predicted);
    }

    points_count += num_points;
  }

  // for (int i = 0; i < nPairs; i++) {
  //   double *Rt_ij = cameraParameter_ij + 6 * i;
  //   double *Rt_i = cameraParameter + 6 * (indices[2 * i]);
  //   double *Rt_j = cameraParameter + 6 * (indices[2 * i + 1]);

  //   fprintf(stderr, "Rt for pairs (%d, %d)\n", indices[2 * i], indices[2 * i + 1]);
  //   for (int j = 0; j < 6; j++) {
  //     fprintf(stderr, "%0.4f %0.4f %0.4f\n", Rt_ij[j], Rt_i[j], Rt_j[j]);
  //   }
  //   fprintf(stderr, "\n");
  // }

  solvers[0]->solve();
  solvers[1]->solve();

  for (int i = 0; i < frames.size(); i++) {
    AngleAxisAndTranslationToTransformationMatrix(cameraParameter + 6 * i, frames[i]->Rt_absolute);
    frames[i]->transformPointCloudCameraToWorld();
    if (i % 8 == 0) frames[i]->writePointCloud();
  }
}
/*
void ReplicateMatlabStrategy::processLastFrame() {

  // trainBoF();
  // bag = new cuBoF("../result/kdtree/centers.bof");
  // bag = new cuBoF("matlab.bof");
  bag = new cuBoF("tmp.bof");
  vector<float *> histograms;
  float sqrt2 = sqrt(2);

  vector<int> match1;
  vector<int> match2;

  cerr << "Computing histograms" << endl;
  for (int i = 0; i < frames.size(); i++) {
    float *histogram = bag->vectorize(&frames[i]->pairs[0]->siftData);

    // SiftData *data = new SiftData();
    // string path = "../result/sift/sift" + to_string(i + 1);
    // ReadVLFeatSiftData(*data, path.c_str());
    // float *histogram = bag->vectorize(data);

    histograms.push_back(histogram);
  }

  for (int i = 0; i < frames.size(); i++) {
    cerr << "Processing frame " << i << " of " << frames.size() << endl;

    vector<float> weights(frames.size(), 0);
    for (int j = 0; j < frames.size(); j++) {
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
      cerr << "Pair found (" << maxOrigScore << ", " << maxWeightedScore << ") at (" << i << ", " << maxIndex << ") with weight " << weights[maxIndex] << endl; 
      match1.push_back(i);
      match2.push_back(maxIndex);
    }
  }

  // Free histogram data
  for (int i = 0; i < frames.size(); i++) {
    free(histograms[i]);
  }

  // Time-based
  cerr << "Build pose graph and bundle adjustment problems" << endl;
  int nPairs = (frames.size() - 1) + match1.size();
  double *Rt_relative = new double[6 * nPairs];
  double *Rt_absolute = new double[6 * frames.size()];
  double weight[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  double weightBA[3] = {1, 1, 1};
  vector<SiftMatch *> matches;
  vector<int> matchIndices_i;
  vector<int> matchIndices_j;

  for (int i = 0; i < frames.size() - 1; i++) {
    TransformationMatrixToAngleAxisAndTranslation(frames[i]->Rt_absolute, Rt_absolute + 6 * i);
  }

  for (int n = 0; n < nPairs; n++) {
    bool timeBased = n < (frames.size() - 1);
    cerr << "Computing for pair " << n << " between " << to_string(timeBased ? n : match1[n - frames.size() + 1]) << " and " << to_string(timeBased ? (n + 1) : match2[n - frames.size() + 1]) << endl;
    double *Rt_ij = Rt_relative + 6 * n;
    double *Rt_i = Rt_absolute + 6 * (timeBased ? n : match1[n - frames.size() + 1]);
    double *Rt_j = Rt_absolute + 6 * (timeBased ? (n + 1) : match2[n - frames.size() + 1]);

    vector<SiftMatch *> currMatches;
    if (timeBased) {
      currMatches = frames[n]->matches;      
      TransformationMatrixToAngleAxisAndTranslation(frames[n]->Rt_relative, Rt_ij);
    } else {
      float tmpRt_ij[12];
      currMatches = frames[match1[n - frames.size() + 1]]->computeRelativeTransform(frames[match2[n - frames.size() + 1]], tmpRt_ij);
      TransformationMatrixToAngleAxisAndTranslation(tmpRt_ij, Rt_ij);
    }

    matches.insert(matches.end(), currMatches.begin(), currMatches.end());
    vector<int> currMatchIndex_i(currMatches.size(), timeBased ? n : match1[n - frames.size() + 1]);
    vector<int> currMatchIndex_j(currMatches.size(), timeBased ? n + 1 : match2[n - frames.size() + 1]);
    matchIndices_i.insert(matchIndices_i.end(), currMatchIndex_i.begin(), currMatchIndex_i.end());
    matchIndices_j.insert(matchIndices_j.end(), currMatchIndex_j.begin(), currMatchIndex_j.end());

    PoseGraphResidual::AddResidualBlock(solvers[0], Rt_ij, weight, Rt_i, Rt_j);
  }

  // TODO: don't need points_i/j
  double *points_i = new double[3 * matches.size()];
  double *points_j = new double[3 * matches.size()];
  double *points_predicted = new double[3 * matches.size()];
  for (int m = 0; m < matches.size(); m++) {
    double *Rt_i = Rt_absolute + 6 * matchIndices_i[m];
    double *Rt_j = Rt_absolute + 6 * matchIndices_j[m];

    points_i[3 * m + 0] = matches[m]->pt1->coords3D[0];
    points_i[3 * m + 1] = matches[m]->pt1->coords3D[1];
    points_i[3 * m + 2] = matches[m]->pt1->coords3D[2];
    points_j[3 * m + 0] = matches[m]->pt2->coords3D[0];
    points_j[3 * m + 1] = matches[m]->pt2->coords3D[1];
    points_j[3 * m + 2] = matches[m]->pt2->coords3D[2];

    AngleAxisRotateAndTranslatePoint(Rt_i, points_i + 3 * m, points_predicted + 3 * m);
    BundleAdjustmentResidual::AddResidualBlock(solvers[1], points_i + 3 * m, weightBA, Rt_i, points_predicted + 3 * m);
    BundleAdjustmentResidual::AddResidualBlock(solvers[1], points_j + 3 * m, weightBA, Rt_j, points_predicted + 3 * m);
  }
  // double Rt_ij[12] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0 };
  // double weight2[9] = { 100, 100, 100, 100, 100, 100, 100, 100, 100 };
  // PoseGraphResidual::AddResidualBlock(solvers[0], Rt_ij, weight2, Rt_absolute, Rt_absolute + 6 * (frames.size() - 2));

  // TODO: Update time-based relative?

  cerr << "Solving pose graph..." << endl;
  // solvers[0]->solve();

  cerr << "Solving bundle adjustment..." << endl;
  solvers[1]->solve();

  cerr << "Converting back results..." << endl;
  for (int i = 0; i < frames.size(); i++) {
    AngleAxisAndTranslationToTransformationMatrix(Rt_absolute + 6 * i, frames[i]->Rt_absolute);
    frames[i]->transformPointCloudCameraToWorld();
    if (i % 8 == 0) frames[i]->writePointCloud();
  }

  delete [] Rt_relative;
  delete [] Rt_absolute;
}

*/

void ReplicateMatlabStrategy::trainBoF() {
  bag = new cuBoF(REPLICATEMATLABSTRATEGY_NUM_FEATURES, frames.size());

  vector<SiftData *> siftData;
  int totalNumSIFT = 0;

  for (int i = 0; i < frames.size(); i++) {
    SiftData *data = &(frames[i]->pairs[0]->siftData);
    totalNumSIFT += data->numPts;
    siftData.push_back(data);
    // for (int j = 0; j < data->numPts; j++) {
    //   for (int k = 0; k < 128; k++) {
    //     if (isnan(data->h_data[j].data[k])) cerr << i << ", " << j << ", " << k << endl;
    //   }
    // }
  }

  cerr << "Training with " << frames.size() << " frames and " << totalNumSIFT << " sift points" << endl;
  bag->train(siftData, totalNumSIFT);
  bag->save("tmp.bof");
}