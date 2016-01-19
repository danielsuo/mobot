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

using namespace std;
using namespace ceres;

const int REPLICATEMATLABSTRATEGY_NUM_FEATURES = 4000;

class ReplicateMatlabStrategy : public Strategy {
public:
  Cerberus *solver;
  cuBoF *bag;
  
  ReplicateMatlabStrategy();
  ~ReplicateMatlabStrategy();
  virtual void processLastFrame() override;

protected:
  virtual void trainBoF();
};

#endif