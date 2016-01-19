#include "ReplicateMatlabStrategy.h"

ReplicateMatlabStrategy::ReplicateMatlabStrategy() : Strategy() {
  solver = new Cerberus();
}

ReplicateMatlabStrategy::~ReplicateMatlabStrategy() {
  delete bag;
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
    // cerr << "Processing frame " << i << endl;
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

      // cerr << setiosflags(ios::fixed) << setprecision(2) << weight << " ";

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
  int nPairs = frames.size() + match1.size();
  double *Rt_relative = new double[6 * nPairs];
  double *Rt_absolute = new double[6 * frames.size()];
  double weight[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

  cerr << "Computing angle axis and adding residual blocks for time-based" << endl;
  for (int i = 0; i < frames.size() - 1; i++) {
    double *Rt_ij = Rt_relative + 6 * i;
    double *Rt_i  = Rt_absolute + 6 * i;
    double *Rt_j  = Rt_absolute + 6 * (i + 1);

    TransformationMatrixToAngleAxisAndTranslation(frames[i]->Rt_relative, Rt_ij);
    TransformationMatrixToAngleAxisAndTranslation(frames[i]->Rt_absolute, Rt_i);
    TransformationMatrixToAngleAxisAndTranslation(frames[i + 1]->Rt_absolute, Rt_j);
    
    PoseGraphResidual::AddResidualBlock(solver, Rt_ij, weight, Rt_i, Rt_j);
  }

  cerr << "Add residual blocks for loop closures" << endl;
  for (int i = 0; i < match1.size(); i++) {
    float *tmpRt_ij = new float[12];
    frames[match1[i]]->computeRelativeTransform(frames[match2[i]], tmpRt_ij);

    double *Rt_ij = Rt_relative + 6 * (frames.size() + i);
    double *Rt_i  = Rt_absolute + 6 * match1[i];
    double *Rt_j  = Rt_absolute + 6 * match2[i];

    TransformationMatrixToAngleAxisAndTranslation(tmpRt_ij, Rt_ij);
    PoseGraphResidual::AddResidualBlock(solver, Rt_ij, weight, Rt_i, Rt_j);
  }

  // TODO: Update time-based relative?

  cerr << "Solving..." << endl;
  solver->solve();

  cerr << "Converting back results..." << endl;
  for (int i = 0; i < frames.size(); i++) {
    AngleAxisAndTranslationToTransformationMatrix(Rt_absolute + 6 * i, frames[i]->Rt_absolute);
    frames[i]->transformPointCloudCameraToWorld();
    if (i % 8 == 0) frames[i]->writePointCloud();
  }

  delete [] Rt_relative;
  delete [] Rt_absolute;
}

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