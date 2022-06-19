#include <catch2/catch.hpp>
#include "ranges.h"
#include "chunk_view.h"
#include "join_with_view.h"

#include <array>
#include <vector>
#include <string_view>

using namespace Catch::Matchers;

template <rng::range R>
    requires
        std::is_convertible_v<rng::range_value_t<R>, std::string::value_type>
inline std::string to_string(R&& r)
{
    if constexpr (rng::common_range<R> && rng::random_access_range<R>)
    {
        return std::string(rng::begin(r), rng::end(r));
    }
    else
    {
        std::string s;

        if constexpr (rng::sized_range<R>)
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
    auto v = rng::join_with(vec, '-');

    using vt = decltype(v);
    STATIC_REQUIRE(rng::view<vt>);

    CHECK_THAT(to_string(v), Equals("a-b-c"s));

    auto piped = vec | rng::join_with('/');
    STATIC_REQUIRE(rng::view<decltype(piped)>);
    CHECK_THAT(to_string(piped), Equals("a/b/c"s));
}

#if 0
TEST_CASE("interleave", "[views]")
{
    using namespace std::literals;

    std::vector matrix{
        std::vector{'1', '2', '3' },
        std::vector{'4', '5', '6' },
        std::vector{'7', '8', '9' },
    };

    auto v = interleave(matrix);
    CHECK_THAT(to_string(v), Equals("147258369"s));

    auto blocks = matrix | interleave() | chunk(3);
    
    auto it = blocks.begin();
    CHECK_THAT(to_string(*it), Equals("147"s));
}
#endif