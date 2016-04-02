#include <range/v3/all.hpp>
#include <vector>
#include <iostream>
#include <string>

using namespace ranges;
 
constexpr const char *LINE_SEP = "-------------------------";

auto print_line = [] (auto line)
{
    std::cout << "| ";
    for_each(line | view::chunk(3) | view::join('|'), [] (char c) { std::cout << c << ' '; });
    std::cout << "|\n";
};

auto print_lines = [] (auto lines)
{
    std::cout << LINE_SEP << '\n';
    for_each(lines, print_line);
};

int main(int argc, char *argv[])
{
    auto init_rng = view::ints(0) | view::transform([] (int i) { return '1' + static_cast<char>(i % 9); }) | view::take(81);

    std::vector<char> grid;
    grid = init_rng;

    for_each(grid | view::chunk(9) | view::chunk(3), print_lines);
    std::cout << LINE_SEP << '\n'; 
    return 0;
}
