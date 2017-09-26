#include "solver.h"

#include <iostream>
#include <bitset>

void breakpoint() { *((int*)((void*)0)) = 0; }

namespace sudoku
{
    uint16_t possibilities(auto && cells)
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

    void init_solvermasks(auto && groups)
    {
        for (auto && cells : groups)
        {
            uint16_t mask = possibilities(cells);
            for (Grid::Cell & c : cells)
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
        init_solvermasks(grid->zones());
    }

    void Solver::solve_step()
    {
        while (step_bruteforce());
    }

    bool Solver::step_bruteforce()
    {
        auto && cells = grid_->cells();

        auto it = cells.begin();
        for (; it != cells.end(); ++it)
        {
            auto & c = *it;
            if (c.val() == 0) 
            {
                break;
            }
        }
        
        if (it != cells.end())
        {
            std::cout << "val: " << it->as_char() << '\n';
            std::cout << "solvemask: " << std::bitset<16>(it->solvemask_) << '\n';

            // backtrack
            if (it->solvemask_ == 0)
            {
                do
                {
                    if (it == cells.begin()) breakpoint();
                    --it;
                    if (!it->fixed_)
                    {
                        auto mask = it->solvemask_ >> (it->val() - 1);
                        std::cout << "bt idx: " << std::distance(cells.begin(), it) << '\n';
                        std::cout << "bt val: " << it->as_char() << '\n';
                        std::cout << "bt solvemask: " << std::bitset<16>(it->solvemask_) << '\n';
                        if (mask != 0)
                        {
                            break;
                        }
                        else
                        {
                            it->set(0);

                            // inefficient
                            init_solvermasks(grid_->rows());
                            init_solvermasks(grid_->columns());
                            init_solvermasks(grid_->zones());

                            print_grid(*grid_);
                        }
                    }
                }
                while (true);
            }

            auto mask = it->solvemask_ >> it->val();
            if (mask == 0) { breakpoint(); }

            for(int x = 1; x < 9; ++x)
            {
                if ((mask >> (x - 1)) & 1 != 0)
                {
                    std::cout << "x: " << x << '\n';
                    it->set(it->val() + x);
                    std::cout << "val: " << it->as_char() << '\n';

                    // inefficient
                    init_solvermasks(grid_->rows());
                    init_solvermasks(grid_->columns());
                    init_solvermasks(grid_->zones());
                    return true;
                }
            }
        }
        
        return false;
    }
}