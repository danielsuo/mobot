#ifndef BUNDLEADJUSTALLSTRATEGY_H
#define BUNDLEADJUSTALLSTRATEGY_H

#include <math.h>
#include <iostream>
#include <fstream>
#include "Strategy/Strategy.h"
#include "Cerberus/Cerberus.h"
#include "Cerberus/Residuals.h"
#include "cuBoF/cuBoF.h"
#include "cuBoF/utils.h"

const int BUNDLEADJUSTALLSTRATEGY_NUM_FEATURES = 4000;

class BundleAdjustAllStrategy : public Strategy {
public:
  Cerberus *solver;
  cuBoF *bag;
  
  BundleAdjustAllStrategy();
  ~BundleAdjustAllStrategy();
  virtual void processLastFrame() override;

protected:
  virtual void trainBoF();
};

#endif