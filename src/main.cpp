#include <range/v3/all.hpp>
#include <vector>
#include <iostream>
#include <string>

using namespace ranges;
 
constexpr const char *LINE_SEP = "-------------------------";

class Grid
{
public:
    struct alignas(int8_t) Cell
    {
        uint8_t val_:4;
        bool fixed_:1;
        uint8_t pad_:3;

        char print() const { return val_ + '0'; }
        uint8_t val() const { return val_; }
        operator char() const { return print(); }
    };
    static_assert(sizeof(Cell) == 1);

    Grid()
    {
        int i = 0;
        for (Cell & cell : data_)
        {
            cell.val_ = (i++ % 9) + 1;
        }
    }

    any_view<char> cells() const { return data_; }

    std::array<Cell, 81> data_;
};

int main(int argc, char *argv[])
{
    Grid grid;

    for (auto && lines : grid.cells() | view::chunk(9) | view::chunk(3))
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
    return 0;
}
