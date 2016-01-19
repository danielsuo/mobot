#ifndef STRATEGY_H
#define STRATEGY_H

#include <vector>
#include <iostream>
#include "Frame.h"
#include "Grid.h"
#include "Device.h"

using namespace std;

typedef enum {
  // Time-based reconstruction with no robot movement
  DefaultStrategyType,

  // Wait for all frames to be loaded and then bundle adjust
  BundleAdjustAllStrategyType,

  // Replicating what Matlab does
  ReplicateMatlabStrategyType
} StrategyType;

class Strategy {
public:
  Grid *grid;
  vector<Frame *> frames;

  ~Strategy();

  void addFrame(Frame *frame);
  virtual void processMostRecentFrame(vector<Device *> &devices);
  virtual void processLastFrame();

  static Strategy *createStrategy(StrategyType type = DefaultStrategyType);

protected:
  Strategy();
};

#endif