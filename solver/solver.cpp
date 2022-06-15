#include "solver.h"

namespace sudoku
{
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

    Solver::Solver(Grid *grid) : grid_(grid) 
    {
        init_solvermasks(grid->rows());
        init_solvermasks(grid->columns());
        //init_solvermasks(grid->zones()); //TODO
    }

    void Solver::solve_step()
    {
        
    }
}