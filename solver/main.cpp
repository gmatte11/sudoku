#include "ranges.h"
#include "grid.h"
#include "solver.h"

#include <fmt/format.h>

#include <array>
#include <string>


void print(Grid const& grid)
{
    constexpr auto line_fmt = "| {} {} {} | {} {} {} | {} {} {} |\n";

    for (auto lines : grid.chars() | views::chunk(9) | views::chunk(3))
    {
        fmt::print("{:->25}\n", "");
        for (auto line : lines)
        {
            fmt::print(line_fmt, line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7], line[8]);
        }
    }
    fmt::print("{:->25}\n", "");
}

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
    (void)argc, (void)argv;

    Grid grid;
    grid.init(test_grid);

    fmt::print("initial grid\n");
    print(grid);

    Solver solver(grid);

    while (!solver.is_solved())
        solver.solve_step();

    fmt::print("\nfinal grid\n");
    print(grid);

    fmt::print("\nsolved in {} steps.\n", solver.solve_steps_);

#if 0
    auto print_zone = [] (auto&& r, std::string_view name, int idx) 
    {
        constexpr auto format = "{} {}: | {} {} {} | {} {} {} | {} {} {} |\n";
        fmt::print(format, name, idx, r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8]);
    };

    auto as_char = [](auto& c){ Grid::Cell const& cell = c; return cell.as_char(); };

    print_zone(grid.row(0) | ranges::views::transform(as_char), "row", 0);
    print_zone(grid.col(2) | ranges::views::transform(as_char), "col", 2);
    print_zone(grid.zone(0) | ranges::views::transform(as_char), "zone", 0);
#endif

    return 0;
}
