/*******************************************************************************
 * main.cpp
 * --------
 * Where all the magic happens...
 *
 ******************************************************************************/

#include "debug.h"

int main(int argc, char *argv[]) {
  // testCUBOFCreate();
  // readDataFromBlobToMemory();
  buildGrid();
  // compareIndices();
  // testBundleAdjustment(argc, argv);
  return 0;
}

void buildGrid() {
  Grid *grid = new Grid(11, 11);

  for (int j = 4; j < 10; j++) {
    grid->setOccupied(3, j, true);
  }

  grid->print();

  // for (int i = -10; i < 10; i++) {
  //   for (int j = -10; j < 10; j++) {
  //     if (grid->inBounds(i, j)) {
  //       cerr << "Herro!" << i << " " << j << endl;
  //     }
  //   }
  // }
  grid->resize(GridSideLeft, 3);
  grid->print();
  grid->resize(GridSideUp, 2);
  grid->print();
  grid->resize(GridSideRight, 3);
  grid->print();
  grid->resize(GridSideDown, 3);
  grid->print();
  grid->resize(GridSideDown, -3);
  grid->print();


  grid->shift(1, 1);
  grid->print();
  grid->shift(-1, -1);
  grid->print();
  grid->shift(5, 0);
  grid->print();
  grid->move(1, 1);
  grid->print();
  grid->move(-1, -1);
  grid->print();

  // grid->getNextPath();

  Grid *grid2 = new Grid(10, 10);

  grid2->shift(10, 10);

  for (int i = 0; i < grid2->width - 2; i++) {
    for (int j = 0; j < grid2->height - 2; j++) {
      if ((i + j) % 2 == 0) grid2->setOccupied(i, j, true);
    }
  }
  vector<GridPoint *> path = grid2->getNextPath();
  cerr << path.size() << endl;
  grid->integrate(grid2);

  Grid *grid3 = new Grid(5, 5);
  grid3->shift(-1, -4);
  for (int i = 0; i < grid3->width; i++) {
    for (int j = 0; j < grid3->height; j++) {
      grid3->setOccupied(i, j, true);
    }
  }
  grid->integrate(grid3);
}
