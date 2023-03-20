#pragma once

#include <ranges>
#include <iterator>
#include <concepts>
#include <algorithm>

#define USE_RANGEV3 0
#define USE_NANORANGE 0
#define USE_STDRANGE 1

namespace ranges
{
    using namespace std::ranges;

    inline namespace views
    {
        using namespace std::ranges::views;
    }

    namespace detail
    {
        template <typename>
        inline constexpr bool pipeable_v = false;

        template <typename T>
        concept pipeable = pipeable_v<T>;

        template <pipeable LHS, pipeable RHS>
        struct piping
        {
            constexpr piping(LHS&& lhs, RHS&& rhs)
                : lhs_(std::move(lhs))
                , rhs_(std::move(rhs))
            {
            }

            template <viewable_range R>
            constexpr auto operator()(R&& r)
            {
                return rhs_(lhs_(std::move(r)));
            }

        private:
            LHS lhs_;
            RHS rhs_;
        };

        template <typename LHS, typename RHS>
        inline constexpr bool pipeable_v<piping<LHS, RHS>> = true;

        template <typename F>
        struct piped : F
        {
            constexpr explicit piped(F&& f) : F(std::move(f)) {}
        };

        template <typename F>
        inline constexpr bool pipeable_v<piped<F>> = true;

        template <viewable_range R, pipeable F>
            requires (!pipeable<std::remove_cvref_t<R>>)
        constexpr auto operator|(R&& lhs, F&& rhs)
        {
            return std::move(rhs)(std::move(lhs));
        }

        template <pipeable LHS, pipeable RHS>
        constexpr auto operator|(LHS&& lhs, RHS&& rhs)
        {
            return piping(std::move(lhs), std::move(rhs));
        }
    }
};

namespace rng = ranges;