#include "Grid.h"

Grid::Grid(int width, int height) {
  this->width = width;
  this->height = height;

  // Integer cast
  originX = width / 2;
  originY = height / 2;
  robotX = originX;
  robotY = originY;

  grid = vector<vector<bool>>(height, vector<bool>(width));

  fprintf(stderr, "Created grid with width %lu and height %lu\n", grid[0].size(), grid.size());
}

Grid::~Grid() {}

void Grid::shift(int x, int y) {
  resize(GridSideLeft, -x);
  resize(GridSideRight, x);
  resize(GridSideUp, -y);
  resize(GridSideDown, y);

  robotX += x;
  robotY += y;
}

void Grid::resize(GridSide side, int n) {
  if (n > 0) grow(side, n);
  else if (n < 0) shrink(side, -n);
}

void Grid::grow(GridSide side, int n) {
  switch(side) {
    case GridSideLeft:
    for (int i = 0; i < height; i++) {
      grid[i].insert(grid[i].begin(), n, true);
    }
    width += n;
    originX += n;
    robotX += n;
    break;
    case GridSideUp:
    grid.insert(grid.begin(), n, vector<bool>(width, true));
    height += n;
    originY += n;
    robotY += n;
    break;
    case GridSideRight:
    for (int i = 0; i < height; i++) {
      grid[i].insert(grid[i].end(), n, true);
    }
    width += n;
    break;
    case GridSideDown:
    grid.insert(grid.end(), n, vector<bool>(width, true));
    height += n;
    break;
  }
}

void Grid::shrink(GridSide side, int n) {
  switch(side) {
    case GridSideLeft:
    n = min(n, width);
    for (int i = 0; i < height; i++) {
      grid[i].erase(grid[i].begin(), grid[i].begin() + n);
    }
    width -= n;
    originX -= n;
    robotX -= n;
    break;
    case GridSideUp:
    n = min(n, height);
    grid.erase(grid.begin(), grid.begin() + n);
    height -= n;
    originY -= n;
    robotY -= n;
    break;
    case GridSideRight:
    n = min(n, width);
    for (int i = 0; i < height; i++) {
      grid[i].erase(grid[i].end() - n, grid[i].end());
    }
    width -= n;
    break;
    case GridSideDown:
    n = min(n, height);
    grid.erase(grid.end() - n, grid.end());
    height -= n;
    break;
  }
}

void Grid::set(int x, int y, bool val) {
  grid[x][y] = val;
}

bool Grid::get(int x, int y) {
  return grid[x][y];
}

void Grid::print() {
  fprintf(stderr, "Current grid layout:\n");

  // Print header
  fprintf(stderr, "    ");
  for (int j = 0; j < width; j++) {
    fprintf(stderr, "%4d", j - originX);
  }
  fprintf(stderr, "\n");

  for (int i = 0; i < height; i++) {
    fprintf(stderr, "%4d", i - originY);
    for (int j = 0; j < width; j++) {

      if (i == robotY && j == robotX) {
        fprintf(stderr, "   X");
        continue;
      }

      if (grid[i][j]) {
        fprintf(stderr, "   .");
      } else {
        fprintf(stderr, "    ");
      }
    }
    fprintf(stderr, "\n");
  }
}