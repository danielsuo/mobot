#ifndef GRID_H
#define GRID_H

#include <stdio.h>
#include <vector>
#include <iostream>
#include <limits>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;

typedef enum {
  GridSideLeft,
  GridSideUp,
  GridSideRight,
  GridSideDown
} GridSide;

class GridPoint;

class GridPoint {
public:
  int x;
  int y;

  // Has the robot visited this grid point
  bool visited;

  // Is this grid point occupied by stuff
  bool occupied;
  float occupiedConfidence;
  bool buffered;
  float bufferedConfidence;

  // Has our path search algorithm checked this point yet
  bool frontiered;

  // What was the immediately preceding GridPoint to this one when traveling
  GridPoint *came_from;

  // How far is this grid point (Manhattan distance) from our robot's current
  // location?
  float distance;

  vector<GridPoint *> neighbors;

  GridPoint(int x, int y);
  ~GridPoint();

  bool hasNeighbor(int x, int y);
};

class Grid {
public:
  int width;
  int height;

  int originX;
  int originY;

  int robotX;
  int robotY;

  // Shift origin by x, y; robot is always in the center of the grid, not
  // necessarily at the origin
  Grid(int width, int height, int x = 0, int y = 0);
  ~Grid();

  // Change grid size
  void shift(int x, int y);
  void resize(GridSide side, int n);
  void grow(GridSide side, int n);
  void shrink(GridSide side, int n);

  // Query and update grid
  void updateNeighbors();
  GridPoint *get(int x, int y, bool mapCoords = false);
  void setOccupied(int i, int j, bool val);
  bool getOccupied(int i, int j);
  void setVisited(int i, int j, bool val);
  bool getVisited(int i, int j);
  bool inBounds(int x, int y, bool mapCoords = false);
  float getWeight(GridPoint *point1, GridPoint *point2);
  void print();

  // Movement
  vector<GridPoint *> getNextPath();
  void move(int x, int y);

  // Integration
  void integrate(Grid *other);

  // Turn point cloud into grid with resolution (mm) and max distance (half
  // square side length in m) and floor height (m)
  static Grid *gridFromMat(cv::Mat mat, int headingX = 0, int headingY = 1, 
    float worldOriginX = 0, float worldOriginZ = 0, int resolution = 250, 
    float maxDistance = 3.0, float floorHeight = 1.3, int skip = 1);

private:
  vector<vector<GridPoint *>> grid;
};

#endif