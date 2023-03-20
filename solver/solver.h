#pragma once

#include "grid.h"

class Solver
{
public:
    Solver(Grid* grid);

    void solve_step();

    bool is_solved() const;

private:
    Grid* grid_;
};
