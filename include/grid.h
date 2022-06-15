#pragma once

#include "ranges.h"

#include "chunk_view.h"
#include "interleave_view.h"

#include <array>
#include <experimental/resumable>

namespace sudoku
{
    namespace views
    {
        inline auto slice(int p, int n) { return ranges::views::drop(p) | ranges::views::take(n); }
        /*inline auto chunk(int n) 
        {
            return ranges::detail::rao_proxy{
                [n](auto&& r) -> decltype(ranges::subrange(ranges::begin(r), ranges::end(r)))
                {  
                    auto&& it = ranges::begin(r);
                    do
                    {
                        auto&& sub = ranges::subrange(it, ranges::end(r)) | ranges::views::take(n);

                        if (ranges::size(sub) < n)
                            break;

                        co_yield sub;
                        it += ranges::size(sub);
                    } while (it != ranges::end(r));

                    co_return ranges::subrange(it, ranges::end(r));
                }};
        }*/

        inline auto interleave()
        {
            return ranges::detail::rao_proxy{
                [](auto&& r) -> ranges::range_value_t<ranges::range_value_t<decltype(r)>>
                {
                    using R = decltype(r);

                    static_assert(ranges::sized_range<R>);
                    static_assert(ranges::sized_range<ranges::range_value_t<R>>);

                    ranges::range_difference_t<R> max = nano::max(r, ranges::size);

                    for (int i : ranges::iota(0, max))
                    {
                        for (auto sub : r)
                        {
                            co_yield *(sub[i % ranges::size(sub)]);
                        }
                    }
                }};
        }

        inline auto join()
        {
            return ranges::detail::rao_proxy{
                [](auto&& r)
                {
                    return ranges::views::join(r);
                }
            };
        }

        inline auto rows() { return views::chunk(9); }
        inline auto cols() { return views::rows() /*| views::interleave() | views::chunk(9)*/; }
        inline auto zone() 
        {
            return views::chunk(3) | views::chunk(9) 
                 //| ranges::for_each([](auto&& rng) { return std::move(rng) | chunk(3) | interleave(); }) 
                 /*| views::join()*/ | views::chunk(9);
        }
    }

    class Grid
    {
    public:

        struct /*alignas(int8_t)*/ Cell
        {
            uint8_t val_:4;
            bool fixed_:1;
            uint16_t solvemask_:9;
            uint8_t pad_:2;

            Cell()
                : val_(0)
                , fixed_(false)
                , solvemask_(0)
            {
            }

            void init(uint8_t val)
            {
                set(val);
                fixed_ = (val != 0);
            }

            void set(uint8_t val) 
            {
                val_ = val;
                solvemask_ = (val == 0) ? 0b111111111 : 0;
            }

            char as_char() const { return (val_ != 0) ? val_ + '0' : '_'; }
            uint8_t val() const { return val_; }

            operator char() const { return as_char(); }
        };
        //static_assert(sizeof(Cell) == 2);

        auto cells() { return ranges::views::all(data_); }

        template <typename Rng>
        void init(Rng&& values)
        {
            auto it = values.begin();
            for (Cell & c : data_)
            {
                int i = *it++;
                c.init(i);
            }
        }

        auto chars() const 
        { 
            return data_ | ranges::views::transform([](Cell const& c) -> char {return c; }); 
        }

        auto rows()
        {
            return cells() | views::rows();
        }

        auto columns()
        {
            return cells() | views::cols();
        }

        auto zones()
        {
            return cells() | views::zone();
        }

        std::array<Cell, 81> data_;
    };
}
