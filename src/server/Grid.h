#ifndef GRID_H
#define GRID_H

#include <stdio.h>
#include <vector>

using namespace std;

typedef enum {
  GridSideLeft,
  GridSideUp,
  GridSideRight,
  GridSideDown
} GridSide;

class GridPoint {
public:
  bool visited;
  bool occupied;

  GridPoint();
};

class Grid {
public:
  int width;
  int height;

  int originX;
  int originY;

  int robotX;
  int robotY;

  // mm / unit
  int resolution;

  Grid(int width, int height);
  ~Grid();

  // Shift grid by x (width)
  void shift(int x, int y);
  void resize(GridSide side, int n);
  void grow(GridSide side, int n);
  void shrink(GridSide side, int n);
  void move(int x, int y);

  void setOccupied(int x, int y, bool val);
  bool getOccupied(int x, int y);
  void setVisited(int x, int y, bool val);
  bool getVisited(int x, int y);
  void print();


private:
  vector<vector<GridPoint *>> grid;
};

#endif