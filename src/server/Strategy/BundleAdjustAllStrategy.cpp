#include "BundleAdjustAllStrategy.h"

BundleAdjustAllStrategy::BundleAdjustAllStrategy() : Strategy() {
  solver = new Cerberus();
}

BundleAdjustAllStrategy::~BundleAdjustAllStrategy() {
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

void BundleAdjustAllStrategy::processLastFrame() {

  bag = new cuBoF("matlab.bof");
  // trainBoF();
  vector<float *> histograms;
  vector<float> weights;
  float sqrt2 = sqrt(2);

  vector<int> match1;
  vector<int> match2;

  for (int i = 0; i < frames.size(); i++) {
    // cerr << "Processing frame " << i << endl;
    float *histogram = bag->vectorize(&frames[i]->pairs[0]->siftData);

    float maxWeightedScore = 0;
    float maxOrigScore = 0;
    int maxIndex = -1;

    vector<float> scores;
    vector<float> weightedScores;

    ofstream myfile;
    myfile.open("../result/matches/scores_" + to_string(i) + ".csv");
    for (int j = 0; j < histograms.size(); j++) {
      float origScore = dot(histogram, histograms[j], bag->numFeatures);
      float weightedScore = origScore * weights[histograms.size() - j - 1];
      if (weightedScore > maxWeightedScore) {
      // if (origScore > maxOrigScore) {
        maxOrigScore = origScore;
        maxWeightedScore = weightedScore;
        maxIndex = j;
      }
      scores.push_back(origScore);
      weightedScores.push_back(weightedScore);
      myfile << origScore << "," << weightedScore << "," << weights[histograms.size() - j - 1] << endl;
    }
    myfile.close();

    // for (auto j: sort_indexes(scores)) {
      // cerr << "  Scores ( " << origScore << ", " << weightedScore << ") at (" << i << ", " << j << ")" << endl;
      // cerr << "Score (" << scores[j] << ", " << weightedScores[j] << ") with weight " << weights[histograms.size() - j - 1] << " at " << j << endl;
    // }
    // Magic numbers (replicating matlab result)
    // if (maxOrigScore > 0.1) {
    if (maxWeightedScore > 0.2) {
    // if (maxIndex > -1) {
      cout << "Pair found (" << maxOrigScore << ", " << maxWeightedScore << ") at (" << i << ", " << maxIndex << ") with weight " << weights[histograms.size() - maxIndex - 1] << endl; 
      match1.push_back(i);
      match2.push_back(maxIndex);

      // Mat tmp = combineMatchedImages(frames[i]->pairs[0]->gray, frames[maxIndex]->pairs[0]->gray);
      // imwrite("../result/matches/" + to_string(i) + "_" + to_string(maxIndex) + ".jpg", tmp);
    }

    histograms.push_back(histogram);

    // Calculate as bwdist from Matlab would
    float weight = i / 2 + 1;
    if (i % 2 != 0) {
      weight = weight * sqrt2;
    } else {
      weight = sqrt(weight * weight + (weight - 1) * (weight - 1));
    }

    // Divide by 150 because magic numbers!! (Replicating matlab result)
    weights.push_back(min(1.0, weight / 150.0));
  }

  for (int i = 0; i < frames.size(); i++) {
    free(histograms[i]);
  }

  solver->solve();
}

void BundleAdjustAllStrategy::trainBoF() {
  bag = new cuBoF(BUNDLEADJUSTALLSTRATEGY_NUM_FEATURES, frames.size());

  vector<SiftData *> siftData;
  int totalNumSIFT = 0;

  for (int i = 0; i < frames.size(); i++) {
    SiftData *data = &(frames[i]->pairs[0]->siftData);
    totalNumSIFT += data->numPts;
    siftData.push_back(data);
    for (int j = 0; j < data->numPts; j++) {
      for (int k = 0; k < 128; k++) {
        if (isnan(data->h_data[j].data[k])) cerr << i << ", " << j << ", " << k << endl;
      }
    }
  }

  bag->train(siftData, totalNumSIFT);
  bag->save("tmp.bof");
}