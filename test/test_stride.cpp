#include <catch2/catch.hpp>
#include "stride_view.h"

#include <array>
#include <vector>

TEST_CASE("stride_view", "[views]")
{
    auto hundred = std::ranges::views::iota(0, 101);

    auto v1 = hundred | ranges::stride(10);

    CHECK(v1.size() == 11);
    CHECK(v1[0] == 0);
    CHECK(v1[1] == 10);
    CHECK(v1[2] == 20);
    CHECK(v1[10] == 100);

    auto v2 = hundred | rng::views::drop(3) | rng::views::take(11) | rng::stride(3);
    CHECK(v2.size() == 4);
    CHECK(v2[0] == 3);
    CHECK(v2[1] == 6);
    CHECK(v2[2] == 9);
    CHECK(v2[3] == 12);
}