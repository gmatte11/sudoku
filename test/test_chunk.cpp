#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "chunk_view.h"

#include <array>
#include <vector>

using namespace Catch::Matchers;

template <typename R>
auto to_vector(const R& r)
{
    using value_t = rng::range_value_t<const R>;
    if constexpr (rng::common_range<R>)
    {
        return std::vector<value_t>(rng::begin(r), ranges::end(r));
    }
    else
    {
        std::vector<value_t> v;
        for (const value_t& val : r)
            v.push_back(std::move(val));
        return v;
    }
}

TEST_CASE("chunk of common ranges", "[views]")
{
    std::vector arr{1, 2, 3, 4, 5};
    CHECK(rng::size(arr) == 5);

    auto v = arr | rng::chunk(2);
    STATIC_REQUIRE(std::random_access_iterator<rng::iterator_t<decltype(v)>>);
    STATIC_REQUIRE(rng::random_access_range<decltype(v)>);

    CHECK(rng::size(v) == 3);

    CHECK_THAT(to_vector(v[0]), Equals(std::vector{1, 2}));
    CHECK_THAT(to_vector(v[1]), Equals(std::vector{3, 4}));
    CHECK_THAT(to_vector(v[2]), Equals(std::vector{5}));
}

TEST_CASE("chunk of views")
{
    auto bounded = rng::iota(0, 100);
    auto unbounded = rng::iota(0);

    SECTION("unbounded view")
    {
        auto v = rng::chunk(unbounded, 2);

        using vt = decltype(v);
        STATIC_REQUIRE(rng::view<vt>);
        STATIC_REQUIRE_FALSE(rng::sized_range<vt>);
    
        CHECK_THAT(to_vector(v[2]), Equals(to_vector(rng::iota(2 * 2, 2 * 3)))); // [4, 6[
    }

    SECTION("bounded view")
    {
        auto v = bounded | rng::chunk(25);

        using vt = decltype(v);
        STATIC_REQUIRE(rng::view<vt>);
        STATIC_REQUIRE(rng::sized_range<vt>);
        
        CHECK_THAT(to_vector(v[2]), Equals(to_vector(rng::iota(25 * 2, 25 * 3)))); // [50, 75[
    }

    SECTION("chunk of chunk")
    {
        auto v0 = bounded | rng::chunk(5);
        auto v =  v0 | rng::chunk(5);

        using vt = decltype(v);
        STATIC_REQUIRE(rng::view<vt>);
        STATIC_REQUIRE(rng::random_access_range<vt>);

        CAPTURE(v0);
        CAPTURE(v0[0]);
        CHECK(rng::size(v0) == 20);

        CAPTURE(v);
        CHECK(rng::size(v) == 4);

        CAPTURE(v[2]);
        CHECK(rng::size(v[2]) == 5);
        
        auto compare = rng::iota(25 * 2 + 5 * 2, 25 * 2 + 5 * 3); // [60, 65[
        CHECK_THAT(to_vector(v[2][2]), Equals(to_vector(compare)));

        auto joined = v[2] | rng::join;
        using jt = decltype(joined);
        STATIC_REQUIRE(rng::view<jt>);
        STATIC_REQUIRE(rng::input_range<jt>);

        // joined type defeats to_vector... and can't be captured.
        std::vector<int> vec;
        for (int i : joined)
            vec.push_back(i);
        CAPTURE(vec);

        compare = rng::iota(25 * 2, 25 * 3); // [50, 75[
        CHECK_THAT(vec, Equals(to_vector(compare)));
    }
}