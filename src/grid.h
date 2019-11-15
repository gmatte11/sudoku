#pragma once

#include <range/v3/all.hpp>

#include "chunk_view.h"
#include "interleave_view.h"

namespace sudoku
{
    inline auto rows_view() { return ranges::view::chunk(9); }
    inline auto cols_view() { return rows_view() | interleave() | chunk(9); }
    inline auto zone_view() 
    {
        return ranges::view::chunk(3) | ranges::view::chunk(9) 
             | ranges::view::for_each([](auto&& rng) { return std::move(rng) | chunk(3) | interleave(); }) 
             | ranges::view::join | ranges::view::chunk(9);
    }

    class Grid
    {
    public:

        struct alignas(int8_t) Cell
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
        static_assert(sizeof(Cell) == 2);

        auto cells() { return ranges::view::all(data_); }

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
            return data_ | ranges::view::transform([](Cell const& c) {return (char)c; }); 
        }

        auto rows()
        {
            return cells() | rows_view();
        }

        auto columns()
        {
            return cells() | cols_view();
        }

        auto zones()
        {
            return cells() | zone_view();
        }

        std::array<Cell, 81> data_;
    };
}
