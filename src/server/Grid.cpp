#include "Grid.h"

// TODO
// - Update grid neighbors
// - Properly delete neighbors
// - Iteratively traverse boundary
// - Get path
// - A*
// - Queue of nodes by distance to (recompute)

GridPoint::GridPoint(int x, int y) {
  this->x = x;
  this->y = y;
  visited = false;
  occupied = false;
  fprintf(stderr, "Created grid point at (%d, %d)\n", x, y);
}

Grid::Grid(int width, int height) {
  this->width = width;
  this->height = height;

  // Integer cast
  originX = width / 2;
  originY = height / 2;
  robotX = originX;
  robotY = originY;

  grid = vector<vector<GridPoint *>>(width, vector<GridPoint *>(height));

  // Column major order to support x, y [] access
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      grid[i][j] = new GridPoint(i - originX, j - originY);
    }
  }

  grid[robotX][robotY]->occupied = false;
  grid[robotX][robotY]->visited = true;

  fprintf(stderr, "Created grid with width %lu and height %lu\n", grid.size(), grid[0].size());
}

Grid::~Grid() {
  // Loop through width
  for (int i = 0; i < grid.size(); i++) {
    // Loop through height
    for (int j = 0; j < grid[0].size(); j++) {
      delete grid[i][j];
    }
  }
}

void Grid::shift(int x, int y) {
  if (x != 0) {
    resize(GridSideLeft, -x);
    resize(GridSideRight, x);
  }

  if (y != 0) {
    resize(GridSideUp, -y);
    resize(GridSideDown, y);
  }
}

void Grid::move(int x, int y) {
  robotX += x;
  robotY += y;

  fprintf(stderr, "Robot now at position (%d, %d)\n", robotX, robotY);
  grid[robotX][robotY]->visited = true;
}

void Grid::resize(GridSide side, int n) {
  if (n > 0) grow(side, n);
  else if (n < 0) shrink(side, -n);
}

void Grid::grow(GridSide side, int n) {
  switch(side) {
    case GridSideLeft:
    width += n;
    originX += n;
    robotX += n;
    for (int i = 0; i < n; i++) {
      vector<GridPoint *> tmp = vector<GridPoint *>(height);
      for (int j = 0; j < height; j++) {
        tmp[j] = new GridPoint(n - i - originX - 1, j - originY);
      }
      grid.insert(grid.begin(), 1, tmp);
    }
    break;
    case GridSideUp:
    height += n;
    originY += n;
    robotY += n;
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < n; j++) {
        grid[i].insert(grid[i].begin(), 1, new GridPoint(i - originX, n - j - originY - 1));
      }
    }
    break;
    case GridSideRight:
    for (int i = 0; i < n; i++) {
      vector<GridPoint *> tmp = vector<GridPoint *>(height);
      for (int j = 0; j < height; j++) {
        tmp[j] = new GridPoint(i + width - originX, j - originY);
      }
      grid.insert(grid.end(), 1, tmp);
    }
    width += n;
    break;
    case GridSideDown:
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < n; j++) {
        grid[i].insert(grid[i].end(), 1, new GridPoint(i - originX, j + height - originY));
      }
    }
    height += n;
    break;
  }
}

void Grid::shrink(GridSide side, int n) {
  switch(side) {
    case GridSideLeft:
    n = min(n, width);
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < height; j++) {
        delete grid[i][j];
      }
    }
    grid.erase(grid.begin(), grid.begin() + n);
    width -= n;
    originX -= n;
    robotX -= n;
    break;
    case GridSideUp:
    n = min(n, height);
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < n; j++) {
        delete grid[i][j];
      }
    }
    for (int i = 0; i < width; i++) {
      grid[i].erase(grid[i].begin(), grid[i].begin() + n);
    }
    height -= n;
    originY -= n;
    robotY -= n;
    break;
    case GridSideRight:
    n = min(n, width);
    for (int i = width - n; i < width; i++) {
      for (int j = 0; j < height; j++) {
        delete grid[i][j];
      }
    }
    grid.erase(grid.end() - n, grid.end());
    width -= n;
    break;
    case GridSideDown:
    n = min(n, height);
    for (int i = 0; i < width; i++) {
      for (int j = height - n; j < height; j++) {
        delete grid[i][j];
      }
    }
    for (int i = 0; i < width; i++) {
      grid[i].erase(grid[i].end() - n, grid[i].end());
    }
    height -= n;
    break;
  }
}

// GridPoint *Grid::createGridPoint(int x, int y) {
//   GridPoint *point = new GridPoint(x, y);
//   fprintf(stderr, "Created grid point at (%d, %d)\n", point->x, point->y);
//   // grid[x + originX][y + originY] = point;
//   return point;
// }

bool Grid::inBounds(int x, int y) {
  int gridX = x + originX;
  int gridY = y + originY;
  return gridX >= 0 && gridX < grid.size() && gridY >= 0 && gridY < grid[gridX].size();
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
  fprintf(stderr, "        ");
  for (int j = 0; j < width; j++) {
    fprintf(stderr, "%8d", j - originX);
  }
  fprintf(stderr, "\n");

  for (int i = 0; i < height; i++) {
    fprintf(stderr, "%8d ", i - originY);
    for (int j = 0; j < width; j++) {
      fprintf(stderr, "(% 2d,% 2d) ", grid[j][i]->y, grid[j][i]->x);
      // if (i == robotY && j == robotX) {
      //   fprintf(stderr, "   X");
      //   continue;
      // }

      // if (grid[j][i]->visited) {
      //   fprintf(stderr, "   V");
      // } else if (grid[j][i]->occupied) {
      //   fprintf(stderr, "   .");
      // } else {
      //   fprintf(stderr, "    ");
      // }
    }
    fprintf(stderr, "\n");
  }
}