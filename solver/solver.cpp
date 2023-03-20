#include "solver.h"

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

        constexpr std::array<int, 9> zone_indexes = { 0, 3, 6, 27, 30, 33, 54, 57, 60 };
        std::array<int, 9> indexers = { idx, idx - 1, idx - 2, idx - 9, idx - 10, idx - 11, idx - 18, idx - 19, idx - 20 };

        const int zone_idx = (int)std::distance(zone_indexes.begin(), rng::find_first_of(zone_indexes, indexers));
        auto zone = grid.zone(zone_idx);

        auto check = [=](Cell& cell) { return cell.val() == value; };

        return rng::none_of(row, check)
            && rng::none_of(col, check)
            && rng::none_of(zone, check);
    }

    auto recursive_backtrack(int idx, Grid& grid)
    {
        auto cells = grid.cells();
        auto rev = std::ranges::views::reverse(rng::take_view(cells, idx));
        auto prev = std::ranges::find_if(rev, [](Cell& cell){ return !cell.fixed_; });

        if (prev == cells.rend())
            return prev.base();

        Cell& cell = *prev;
        int prev_idx = (int)std::distance(cells.begin(), prev.base()) - 1;

        const int first = cell.val();
        for (uint8_t i = 1; i < 9; ++i)
        {
            const uint8_t val = ((first + i) % 9) + 1;
            if (check_unique(prev_idx, val, grid))
            {
                cell.set(val);
                return prev.base();
            }
        }

        // recurse
        cell.set(0);
        return recursive_backtrack(prev_idx, grid);
    }
}

Solver::Solver(Grid* grid) : grid_(grid)
{
    //init_solvermasks(grid->rows());
    //init_solvermasks(grid->columns());
    //init_solvermasks(grid->zones()); //TODO
}

void Solver::solve_step()
{
    using Cell = Grid::Cell;

    auto val_proj = [](auto& c) { return c.val(); };
    auto cells = grid_->cells();

    auto next = std::ranges::find(cells, 0, val_proj);

    if (next == cells.end())
        return;

    int idx = (int)std::distance(cells.begin(), next) - 1;

    for (uint8_t v = 1; v <= 9; ++v)
    {
        if (check_unique(idx, v, *grid_))
        {
            next->set(v);
            return;
        }
    }

    // backtracking
    recursive_backtrack(idx, *grid_);
}

bool Solver::is_solved() const
{
    return std::ranges::none_of(grid_->cells(), [](Grid::Cell const& c) { return c.val() == 0; });
}