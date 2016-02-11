#ifndef REPLICATEMATLABSTRATEGY_H
#define REPLICATEMATLABSTRATEGY_H

#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "Strategy/Strategy.h"

#include "Cerberus/Cerberus.h"
#include "Cerberus/Residuals.h"
#include "Residuals/BundleAdjustmentResidual.h"
#include "Residuals/PoseGraphResidual.h"

#include "cuBoF/cuBoF.h"
#include "cuBoF/utils.h"
#include "cuSIFT/extras/debug.h"

#include <folly/futures/Future.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/LifoSemMPMCQueue.h>

using namespace std;
using namespace ceres;
using namespace folly;
using namespace wangle;

const int REPLICATEMATLABSTRATEGY_NUM_FEATURES = 4000;
const int MAX_NUM_BA_POINTS = 100;
const int MIN_NUM_MATCH_POINTS_FOR_LOOP_CLOSURE = 20;

class ReplicateMatlabStrategy : public Strategy {
public:
  vector<Cerberus *> solvers;
  cuBoF *bag;
  
  ReplicateMatlabStrategy();
  ~ReplicateMatlabStrategy();
  virtual void processLastFrame() override;

protected:
  void getRt(double *Rt, int index);
  vector<SiftMatch *> getMatches(int i, int j, double *Rt);
  // vector<int> getFramePairs();
  void getFramePairs(vector<int>& indices, vector<vector<SiftMatch *>>& allMatches);
  virtual void trainBoF();
};

#endif