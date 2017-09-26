#include <range/v3/all.hpp>
#include <array>

#include "grid.h"
#include "solver.h"

using namespace ranges;
using namespace sudoku;

std::array<int, 81> test_grid = 
{
    7, 9, 0,  0, 0, 0,  3, 0, 0,
    0, 0, 0,  0, 0, 6,  9, 0, 0,
    8, 0, 0,  0, 3, 0,  0, 7, 6,

    0, 0, 0,  0, 0, 5,  0, 0, 2,
    0, 0, 5,  4, 1, 8,  7, 0, 0,
    4, 0, 0,  7, 0, 0,  0, 0, 0,

    6, 1, 0,  0, 9, 0,  0, 0, 8,
    0, 0, 2,  3, 0, 0,  0, 0, 0,
    0, 0, 9,  0, 0, 0,  0, 5, 4
};

int main(int argc, char *argv[])
{
    Grid grid;
    grid.init(view::all(test_grid));
    print_grid(grid);

    Solver solver(&grid);
    solver.solve_step();
    print_grid(grid);
    return 0;
}
