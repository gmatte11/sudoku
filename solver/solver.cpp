#include "solver.h"
#include "grid.h"

#include <fmt/format.h>

#include <iterator>


namespace 
{
    using Cell = Grid::Cell;

    template <typename R>
    uint16_t possibilities(R&& cells)
    {
        uint16_t mask = 0b111111111;
        for (Grid::Cell & c : cells)
        {
            if (c.val() > 0)
            {
                mask &= ~(1 << (c.val() - 1));
            }
        }

        return mask;
    }

    template <typename V>
    void init_solvermasks(V groups)
    {
        for (auto cells : groups)
        {
            uint16_t mask = possibilities(cells);
            for (Grid::Cell& c : cells)
            {
                if (c.val() == 0)
                {
                    c.solvemask_ &= mask;
                }
            }
        }
    }
    
    bool check_unique(int idx, uint8_t value, Grid& grid)
    {
        const int row_idx = idx / 9;
        auto row = grid.row(row_idx);
        
        const int col_idx = idx % 9;
        auto col = grid.col(col_idx);

        auto zone = grid.zone_of(grid.cells()[idx]);

        auto check = [=](Grid::Cell const& cell) { return cell.val() == value; };

        return ranges::none_of(row, check)
            && ranges::none_of(col, check)
            && ranges::none_of(zone.value(), check);
    }

    int recursive_backtrack(int idx, Grid& grid)
    {
        Cell& cell = grid.cells()[idx];

        const int first = cell.val();
        for (uint8_t val = cell.val() + 1; val <= 9; ++val)
        {
            if (check_unique(cell.idx_, val, grid))
            {
                cell.set(val);
                return idx;
            }
        }

        // recurse
        cell.set(0);
        return recursive_backtrack(grid.prev_idx(cell.idx_), grid);
    }

}

Solver::Solver(Grid& grid) : grid_(grid)
{
    //init_solvermasks(grid->rows());
    //init_solvermasks(grid->columns());
    //init_solvermasks(grid->zones()); //TODO

    next_idx_ = grid_.next_idx(-1);
}

void Solver::solve_step()
{
    Grid::Cell& cell = grid_.cells()[next_idx_];

    bool found = false;
    for (uint8_t v = 1; v <= 9; ++v)
    {
        if (check_unique(cell.idx_, v, grid_))
        {
            cell.set(v);
            found = true;
            break;
        }
    }

    // backtracking
    if (!found)
    {
        next_idx_ = recursive_backtrack(grid_.prev_idx(cell.idx_), grid_);
    }

    next_idx_ = grid_.next_idx(next_idx_);
    ++solve_steps_;
}

bool Solver::is_solved() const
{
    return next_idx_ == -1;
}