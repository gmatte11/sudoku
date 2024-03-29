#include <catch2/catch.hpp>
#include "join_with_view.h"

#include <vector>
#include <string_view>

using namespace Catch::Matchers;

template <ranges::range R>
    requires
        std::is_convertible_v<ranges::range_value_t<R>, std::string::value_type>
inline std::string to_string(R&& r)
{
    if constexpr (ranges::common_range<R> && ranges::random_access_range<R>)
    {
        return std::string(ranges::begin(r), ranges::end(r));
    }
    else
    {
        std::string s;

        if constexpr (ranges::sized_range<R>)
            s.reserve(ranges::size(r));

        for (auto c : r)
            s += c;

        return s;
    }
}

TEST_CASE("join_with", "[views]")
{
    using namespace std::literals;

    std::vector vec{"a"sv, "b"sv, "c"sv};
    auto v = views::join_with(vec, '-');

    using vt = decltype(v);
    STATIC_REQUIRE(ranges::view<vt>);

    CHECK_THAT(to_string(v), Equals("a-b-c"s));

    auto piped = vec | views::join_with('/');
    STATIC_REQUIRE(ranges::view<decltype(piped)>);
    CHECK_THAT(to_string(piped), Equals("a/b/c"s));
}