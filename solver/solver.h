#pragma once

#include <cstdint>

class Grid;

class Solver
{
public:
    Solver(Grid& grid);

    void solve_step();
    bool is_solved() const;

    int64_t solve_steps_ = 0;

private:
    Grid& grid_;
    int next_idx_ = 0;
};
