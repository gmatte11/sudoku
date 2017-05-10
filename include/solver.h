#pragma once

#include "grid.h"

namespace sudoku
{
   class Solver
    {
    public:
        Solver(Grid *grid);

        void solve_step();

    private:
        Grid *grid_;
    };
}
