#include <range/v3/all.hpp>
#include <vector>
#include <iostream>
#include <string>

#include "chunk_view.h"
#include "interleave_view.h"

using namespace ranges;
using namespace sudoku_view;

constexpr const char *LINE_SEP = "-------------------------";


auto&& rows_view = view::chunk(9);
auto&& cols_view = rows_view | interleave() | chunk(9);
auto&& zone_view = view::chunk(3) | view::chunk(9) 
                 | view::for_each([](auto&& rng) { return std::move(rng) | chunk(3) | interleave(); }) 
                 | view::join | view::chunk(9);

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

        char as_char() const { return (val_ != 0) ? val_ + '0' : '_'; }
        uint8_t val() const { return val_; }

        operator char() const { return as_char(); }
    };
    static_assert(sizeof(Cell) == 1);

    auto cells() { return view::all(data_); }

    template <typename Rng>
    void init(Rng&& values)
    {
        auto it = values.begin();
        for (Cell & c : data_)
        {
            int i = *it++;
            c.val_ = i;
            c.fixed_ = (i != 0);
        }
    }

    auto chars() const 
    { 
        return data_ | view::transform([](Cell const& c) {return (char)c; }); 
    }

    auto rows()
    {
        return cells() | rows_view;
    }

    auto columns()
    {
        return cells() | cols_view;
    }

    auto zones()
    {
        return cells() | zone_view;
    }

    std::array<Cell, 81> data_;
};

void print(Grid const& grid)
{
    for (auto && lines : grid.chars() | rows_view | view::chunk(3))
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
    Grid grid;
    grid.init(view::all(test_grid));
    print(grid);
    return 0;
}
