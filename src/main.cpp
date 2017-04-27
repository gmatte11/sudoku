#include <range/v3/all.hpp>
#include <vector>
#include <iostream>
#include <string>

#include "chunk_view.h"
#include "interleave_view.h"

using namespace ranges;
using namespace sudoku_view;

constexpr const char *LINE_SEP = "-------------------------";


class Grid
{
public:
    struct alignas(int8_t) Cell
    {
        uint8_t val_:4;
        bool fixed_:1;
        uint8_t pad_:3;

        Cell()
            : val_(0)
            , fixed_(false)
        {
        }

        char as_char() const { return val_ + '0'; }
        uint8_t val() const { return val_; }

        operator char() const { return as_char(); }
    };
    static_assert(sizeof(Cell) == 1);

    any_view<char> chars() const { return data_; }
    auto cells() { return view::all(data_); }

    auto rows()
    {
        return cells() | view::chunk(9);
    }

    auto columns()
    {
        return rows() | interleave() | chunk(9);
    }

    auto zones()
    {
        return cells() | view::chunk(3) | view::chunk(9) | view::for_each([](auto&& rng) { return rng | chunk(3) | interleave(); }) | view::join | view::chunk(9);
    }

    std::array<Cell, 81> data_;
};

void print(Grid const& grid)
{
    for (auto && lines : grid.chars() | view::chunk(9) | view::chunk(3))
    {
        std::cout << LINE_SEP << '\n';
        for (auto && line : lines)
        {
            std::cout << "| ";
            for (char c : std::move(line) | view::chunk(3) | view::join('|')) { std::cout << c << ' '; }
            std::cout << "|\n";
        }
    }
    std::cout << LINE_SEP << '\n';
}

template <typename Rng>
void fill_cells(Rng && rng)
{
    for (auto && cells : rng)
    {
        auto nums = view::ints(1);
        auto i = std::begin(nums);
        for (auto & cell : cells)
        {
            cell.val_ = *i++;
        }
    }
}

int main(int argc, char *argv[])
{
    Grid grid;

    std::cout << "Rows\n";
    fill_cells(grid.rows());
    print(grid);

    std::cout << "\n\nColumns\n";
    fill_cells(grid.columns());
    print(grid);

    std::cout << "\n\nZones\n";
    fill_cells(grid.zones());
    print(grid);
    return 0;
}
