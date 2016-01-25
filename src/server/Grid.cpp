#include "Grid.h"

// TODO
// - Occupancy confidence
// - Integrate

GridPoint::GridPoint(int x, int y) {
  this->x = x;
  this->y = y;
  visited = false;
  occupied = false;
  frontiered = false;
  came_from = nullptr;

  confidence = 0.0;
  distance = 0.0;
  // fprintf(stderr, "Created grid point at (%d, %d)\n", x, y);
}

GridPoint::~GridPoint() {
  for (int i = 0; i < neighbors.size(); i++) {
    GridPoint *neighbor = neighbors[i];
    for (int j = 0; j < neighbor->neighbors.size(); j++) {
      GridPoint *self = neighbor->neighbors[j];
      if (self->x == x && self->y == y) {
        neighbor->neighbors.erase(neighbor->neighbors.begin() + j);
        // fprintf(stderr, "Remove neighbor (%d, %d) for grid point (%d, %d)\n", x, y, neighbor->x, neighbor->y);
      }
    }
  }
}

bool GridPoint::hasNeighbor(int x, int y) {
  for (int i = 0; i < neighbors.size(); i++) {
    if (neighbors[i]->x == x && neighbors[i]->y == y) {
      return true;
    }
  }
  return false;
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

  updateNeighbors();

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

void Grid::resize(GridSide side, int n) {
  if (n > 0) grow(side, n);
  else if (n < 0) shrink(side, -n);

  updateNeighbors();
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
      grid.insert(grid.begin(), tmp);
    }
    break;
    case GridSideUp:
    height += n;
    originY += n;
    robotY += n;
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < n; j++) {
        grid[i].insert(grid[i].begin(), new GridPoint(i - originX, n - j - originY - 1));
      }
    }
    break;
    case GridSideRight:
    for (int i = 0; i < n; i++) {
      vector<GridPoint *> tmp = vector<GridPoint *>(height);
      for (int j = 0; j < height; j++) {
        tmp[j] = new GridPoint(i + width - originX, j - originY);
      }
      grid.insert(grid.end(), tmp);
    }
    width += n;
    break;
    case GridSideDown:
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < n; j++) {
        grid[i].insert(grid[i].end(), new GridPoint(i - originX, j + height - originY));
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

// Add any missing neighbors. Deconstructor handles dangling neighbors
void Grid::updateNeighbors() {
  float weight = 1.0;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      GridPoint *point = grid[i][j];
      if (i > 0 && !point->hasNeighbor(point->x - 1, point->y)) {
        point->neighbors.push_back(grid[i - 1][j]);
        // fprintf(stderr, "Create neighbor (%d, %d) for grid point (%d, %d)\n", grid[i - 1][j]->x, grid[i - 1][j]->y, point->x, point->y);
      }
      if (i < width - 1 && !point->hasNeighbor(point->x + 1, point->y)) {
        point->neighbors.push_back(grid[i + 1][j]);
        // fprintf(stderr, "Create neighbor (%d, %d) for grid point (%d, %d)\n", grid[i + 1][j]->x, grid[i + 1][j]->y, point->x, point->y);
      }
      if (j > 0 && !point->hasNeighbor(point->x, point->y - 1)) {
        point->neighbors.push_back(grid[i][j - 1]);
        // fprintf(stderr, "Create neighbor (%d, %d) for grid point (%d, %d)\n", grid[i][j - 1]->x, grid[i][j - 1]->y, point->x, point->y);
      }
      if (j < height - 1 && !point->hasNeighbor(point->x, point->y + 1)) {
        point->neighbors.push_back(grid[i][j + 1]);
        // fprintf(stderr, "Create neighbor (%d, %d) for grid point (%d, %d)\n", grid[i][j + 1]->x, grid[i][j + 1]->y, point->x, point->y);
      }
    }
  }
}

GridPoint *Grid::get(int x, int y, bool mapCoords) {
  if (inBounds(x, y, mapCoords)) {
    if (!mapCoords) {
      int gridX = x + originX;
      int gridY = y + originY;
      return grid[gridX][gridY];
    } else {
      return grid[x][y];
    }
  }

  return nullptr;
}

float Grid::getWeight(GridPoint *point1, GridPoint *point2) {
  if (point1->occupied || point2->occupied) {
    return 999;
  } else if (point1->visited || point2->visited) {
    return 2.0;
  } else {
    return 1.0;
  }
}

bool Grid::inBounds(int x, int y, bool mapCoords) {
  int gridX = x + (mapCoords ? 0 : originX);
  int gridY = y + (mapCoords ? 0 : originY);
  return gridX >= 0 && gridX < width && gridY >= 0 && gridY < height;
}

void Grid::setOccupied(int i, int j, bool val) {
  grid[i][j]->occupied = val;
}

bool Grid::getOccupied(int i, int j) {
  return grid[i][j]->occupied;
}

void Grid::setVisited(int i, int j, bool val) {
  grid[i][j]->visited = val;
}

bool Grid::getVisited(int i, int j) {
  return grid[i][j]->visited;
}

void Grid::print() {
  fprintf(stderr, "Current grid layout x->(%d, %d), y->(%d, %d) :\n", 
    -originX, width - originX - 1, -originY, height - originY - 1);

  // Print header
  fprintf(stderr, "     ");
  for (int j = 0; j < width; j++) {
    fprintf(stderr, "%4d", j - originX);
  }
  fprintf(stderr, "\n");

  for (int i = 0; i < height; i++) {
    fprintf(stderr, "%4d ", i - originY);
    for (int j = 0; j < width; j++) {
      // fprintf(stderr, "(% 2d,% 2d) ", grid[j][i]->x, grid[j][i]->y);
      if (i == robotY && j == robotX) {
        fprintf(stderr, "   X");
        continue;
      }

      if (grid[j][i]->visited) {
        fprintf(stderr, "   V");
      // } else if (grid[j][i]->frontiered) {
      //   fprintf(stderr, "   F");
      } else if (grid[j][i]->occupied) {
        fprintf(stderr, "   .");
      } else {
        fprintf(stderr, "    ");
      }
    }
    fprintf(stderr, "\n");
  }
}

vector<GridPoint *> Grid::getNextPath() {
  vector<GridPoint *> path;
  fprintf(stderr, "Robot's position %d %d\n", robotX, robotY);
  GridPoint *start = get(robotX, robotY, true);

  if (start != nullptr) {
    fprintf(stderr, "Robot's current position: (%d, %d)\n", start->x, start->y);

    // Do an exhaustive breadth-first search over all reachable parts of the
    // grid
    vector<GridPoint *> frontier;
    frontier.insert(frontier.begin(), start);
    start->frontiered = true;

    float minDistance = numeric_limits<float>::max();
    GridPoint *goal = nullptr;

    while (frontier.size() > 0) {
      GridPoint *current = frontier.back();
      frontier.pop_back();

      for (int i = 0; i < current->neighbors.size(); i++) {
        GridPoint *next = current->neighbors[i];
        if (!next->frontiered && !next->occupied) {
          frontier.insert(frontier.begin(), next);
          next->frontiered = true;
          next->came_from = current;
          next->distance = current->distance + getWeight(current, next);
          if (!next->visited && next->distance < minDistance) {
            goal = next;
            minDistance = next->distance;
            // fprintf(stderr, "Found next goal: (%d, %d), distance %0.2f\n", next->x, next->y, next->distance);
          }
          // fprintf(stderr, "Visited: (%d, %d), distance %0.2f\n", next->x, next->y, next->distance);
        }
      }
    }

    // goal = grid[1][6];
    // goal->visited = true;
    // print();

    if (goal != nullptr) {
      path.insert(path.begin(), goal);
      while (goal->came_from != start) {
        path.insert(path.begin(), goal->came_from);
        goal = goal->came_from;
        fprintf(stderr, "Path added: (%d, %d)\n", goal->x, goal->y);
      }
    }

    // for (int i = 0; i < path.size(); i++) {
    //   fprintf(stderr, "Path step %d: (%d, %d)\n", i, path[i]->x, path[i]->y);
    // }

    // Reset grid points
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        GridPoint *point = grid[i][j];
        point->frontiered = false;
        point->distance = 0;
        point->came_from = nullptr;
      }
    }
  }

  return path;
}

void Grid::move(int x, int y) {
  robotX += x;
  robotY += y;

  fprintf(stderr, "Robot now at position (%d, %d)\n", robotX, robotY);
  grid[robotX][robotY]->visited = true;
}

void Grid::integrate(Grid *other) {
  // other->print();
  // fprintf(stderr, "Other origin (%d, %d)\n", other->originX, other->originY);
  // fprintf(stderr, "Other robot (%d, %d)\n", other->robotX, other->robotY);

  // Get new boundaries
  int left  = min(-originX, -other->originX);
  int up    = min(-originY, -other->originY);
  int right = max(width - originX - 1, other->width - other->originX - 1);
  int down  = max(height - originY - 1, other->height - other->originY - 1);

  // fprintf(stderr, "Current boundaries x->(%d, %d), y->(%d, %d)\n", 
  //   -originX, width - originX - 1, -originY, height - originY - 1);
  // fprintf(stderr, "New boundaries x->(%d, %d), y->(%d, %d)\n", left, right, up, down);

  // Compute resizing
  int dleft  = -(left + originX);
  int dup    = -(up + originY);
  int dright = right - (width - originX - 1);
  int ddown  = down - (height - originY - 1);

  // fprintf(stderr, "Delta boundaries x->(%d, %d), y->(%d, %d)\n", dleft, dright, dup, ddown);

  // Resize
  resize(GridSideLeft, dleft);
  resize(GridSideUp, dup);
  resize(GridSideRight, dright);
  resize(GridSideDown, ddown);

  // fprintf(stderr, "Current boundaries x->(%d, %d), y->(%d, %d)\n", 
  //   -originX, width - originX - 1, -originY, height - originY - 1);
  // print();

  // Copy occupancy data
  int gx = -other->originX + originX;
  int gy = -other->originY + originY;
  // fprintf(stderr, "Offset (%d, %d)\n", gx, gy);
  for (int i = 0; i < other->width; i++) {
    for (int j = 0; j < other->height; j++) {
      grid[gx + i][gy + j]->occupied |= other->getOccupied(i, j);
    }
  }
  // print();
}
