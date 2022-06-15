#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "chunk_view.h"

#include <array>
#include <vector>

template <typename R>
auto to_vector(const R& r)
    -> std::enable_if_t<ranges::sized_range<R>, std::vector<ranges::range_value_t<R>>>
{
    return std::vector<ranges::range_value_t<R>>(ranges::begin(r), ranges::end(r));
}

TEST_CASE("chunk of common ranges", "[views]")
{
    using namespace sudoku;

    std::vector arr{1, 2, 3, 4, 5};
    CHECK(ranges::size(arr) == 5);

    auto v = arr | views::chunk(2);

    CHECK(ranges::size(v) == 3);

    auto chunk2 = v[1];
    CHECK_THAT(to_vector(chunk2), Catch::Matchers::Equals(std::vector{3, 4}));
}

TEST_CASE("chunk of views")
{
    using namespace sudoku;
    using namespace Catch::Matchers;

    auto bounded = ranges::views::iota(0, 100);
    auto unbounded = ranges::views::iota(0);

    SECTION("unbounded view")
    {
        auto v = views::chunk(unbounded, 2);

        using vt = decltype(v);
        CHECK(ranges::view<vt>);
        CHECK_FALSE(ranges::sized_range<vt>);
    
        CHECK_THAT(to_vector(v[2]), Equals(to_vector(ranges::views::iota(2 * 2, 2 * 3)))); // [4, 6[
    }

    SECTION("bounded view")
    {
        auto v = bounded | views::chunk(25);

        using vt = decltype(v);
        CHECK(ranges::view<vt>);
        CHECK(ranges::sized_range<vt>);
        
        CHECK_THAT(to_vector(v[2]), Equals(to_vector(ranges::views::iota(25 * 2, 25 * 3)))); // [50, 75[
    }

    SECTION("chunk of chunk")
    {
        auto v =  bounded | views::chunk(25) | views::chunk(5);

        using vt = decltype(v);
        CHECK(ranges::view<vt>);
        CHECK(ranges::random_access_range<vt>);

        auto compare = ranges::views::iota((25 / 5) * 2 + 5 * 0, (25 / 5) * 2 + 5 * 1); // [10, 15[
        CHECK_THAT(to_vector(v[2][0]), Equals(to_vector(compare))); 
    }
}