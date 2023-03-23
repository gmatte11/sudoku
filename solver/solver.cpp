#include "solver.h"
#include "grid.h"

#include <fmt/format.h>

#include <iterator>


namespace 
{
    using Cell = Grid::Cell;
    
    bool check_unique(int idx, uint8_t value, Grid& grid)
    {
        const int row_idx = idx / 9;
        auto row = grid.row(row_idx);
        
        const int col_idx = idx % 9;
        auto col = grid.col(col_idx);

        auto zone = grid.zone_of(grid.cells()[idx]);

        auto check = [=](Cell const& cell) { return cell.val() == value; };

        return ranges::none_of(row, check)
            && ranges::none_of(col, check)
            && ranges::none_of(zone.value(), check);
    }

    int try_set_or_backtrack(Cell& cell, Grid& grid)
    {
        for (uint8_t val = cell.val() + 1; val <= 9; ++val)
        {
            if (check_unique(cell.idx_, val, grid))
            {
                cell.set(val);
                return cell.idx_;
            }
        }

        cell.set(0);

        Cell& prev = grid.cells()[grid.prev_idx(cell.idx_)];
        return try_set_or_backtrack(prev, grid);
    }
}

Solver::Solver(Grid& grid) : grid_(grid)
{
    next_idx_ = grid_.next_idx(-1);
}

void Solver::solve_step()
{
    Cell& cell = grid_.cells()[next_idx_];

    cell.set(0);
    int last_set_idx = try_set_or_backtrack(cell, grid_);

    next_idx_ = grid_.next_idx(last_set_idx);
    ++solve_steps_;
}

bool Solver::is_solved() const
{
    return next_idx_ == -1;
}