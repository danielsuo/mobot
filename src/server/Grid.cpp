#include "Grid.h"

GridPoint::GridPoint() {
  visited = false;
  occupied = true;
}

Grid::Grid(int width, int height) {
  this->width = width;
  this->height = height;

  // Integer cast
  originX = width / 2;
  originY = height / 2;
  robotX = originX;
  robotY = originY;

  grid = vector<vector<GridPoint *>>(height, vector<GridPoint *>(width));
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      grid[i][j] = new GridPoint();
    }
  }

  fprintf(stderr, "Created grid with width %lu and height %lu\n", grid[0].size(), grid.size());
}

Grid::~Grid() {
  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid[0].size(); j++) {
      delete grid[i][j];
    }
  }
}

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
      grid[i].insert(grid[i].begin(), n, new GridPoint());
    }
    width += n;
    originX += n;
    robotX += n;
    break;
    case GridSideUp:
    for (int i = 0; i < n; i++) {
      vector<GridPoint *> tmp = vector<GridPoint *>(width);
      for (int j = 0; j < width; j++) {
        tmp[j] = new GridPoint();
      }
      grid.insert(grid.begin(), 1, tmp);
    }
    height += n;
    originY += n;
    robotY += n;
    break;
    case GridSideRight:
    for (int i = 0; i < height; i++) {
      grid[i].insert(grid[i].end(), n, new GridPoint());
    }
    width += n;
    break;
    case GridSideDown:
    for (int i = 0; i < n; i++) {
      vector<GridPoint *> tmp = vector<GridPoint *>(width);
      for (int j = 0; j < width; j++) {
        tmp[j] = new GridPoint();        
      }
      grid.insert(grid.end(), n, tmp);
    }
    height += n;
    break;
  }
}

void Grid::shrink(GridSide side, int n) {
  switch(side) {
    case GridSideLeft:
    n = min(n, width);
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < n; j++) {
        delete grid[i][j];
      }
    }
    for (int i = 0; i < height; i++) {
      grid[i].erase(grid[i].begin(), grid[i].begin() + n);
    }
    width -= n;
    originX -= n;
    robotX -= n;
    break;
    case GridSideUp:
    n = min(n, height);
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < width; j++) {
        delete grid[i][j];
      }
    }
    grid.erase(grid.begin(), grid.begin() + n);
    height -= n;
    originY -= n;
    robotY -= n;
    break;
    case GridSideRight:
    n = min(n, width);
    for (int i = 0; i < height; i++) {
      for (int j = width - n; j < width; j++) {
        delete grid[i][j];
      }
    }
    for (int i = 0; i < height; i++) {
      grid[i].erase(grid[i].end() - n, grid[i].end());
    }
    width -= n;
    break;
    case GridSideDown:
    n = min(n, height);
    for (int i = height - n; i < height; i++) {
      for (int j = 0; j < width; j++) {
        delete grid[i][j];
      }
    }
    grid.erase(grid.end() - n, grid.end());
    height -= n;
    break;
  }
}

void Grid::setOccupied(int x, int y, bool val) {
  grid[x][y]->occupied = val;
}

bool Grid::getOccupied(int x, int y) {
  return grid[x][y]->occupied;
}

void Grid::setVisited(int x, int y, bool val) {
  grid[x][y]->visited = val;
}

bool Grid::getVisited(int x, int y) {
  return grid[x][y]->visited;
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

      if (grid[i][j]->occupied) {
        fprintf(stderr, "   .");
      } else {
        fprintf(stderr, "    ");
      }
    }
    fprintf(stderr, "\n");
  }
}