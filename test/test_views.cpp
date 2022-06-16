#include <catch2/catch.hpp>
#include "ranges.h"

#include <array>
#include <vector>
#include <string_view>

using namespace sudoku::views;
using namespace Catch::Matchers;

inline std::string to_string(auto&& r)
{
    static_assert(rng::range<decltype(r)>);
    static_assert(std::is_convertible_v<rng::range_value_t<decltype(r)>, std::string::value_type>);

    if constexpr (rng::common_range<decltype(r)>)
    {
        return std::string(rng::begin(r), rng::end(r));
    }
    else
    {
        std::string s;

        if constexpr (rng::sized_range<decltype(r)>)
            s.reserve(rng::size(r));

        for (auto c : r)
            s += c;

        return s;
    }
}

TEST_CASE("join_with", "[views]")
{
    using namespace std::literals;

    std::vector vec{"a"sv, "b"sv, "c"sv};
    auto v = join_with(vec, '-');

    using vt = decltype(v);
    STATIC_REQUIRE(rng::view<vt>);

    CHECK_THAT("a-b-c"s, Equals(to_string(v)));

    auto piped = vec | join_with('/');
    STATIC_REQUIRE(rng::view<decltype(piped)>);
    CHECK_THAT("a/b/c"s, Equals(to_string(piped)));
}