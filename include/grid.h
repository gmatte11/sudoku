#pragma once

#include "ranges.h"

#include "chunk_view.h"
#include "stride_view.h"

#include <array>
#include <coroutine>

class Grid
{
public:
    struct Cell
    {
        uint16_t solvemask_;
        uint8_t val_;
        bool fixed_;
        uint8_t idx_;

        Cell()
            : val_(0), fixed_(false), solvemask_(0)
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

    std::span<Cell> cells() { return data_; }
    std::span<const Cell> cells() const { return data_; }

    void init(std::span<int> values)
    {
        uint8_t idx = 0;
        auto it = values.begin();
        for (Cell& c : data_)
        {
            int i = *it++;
            c.init((uint8_t)i);
            c.idx_ = idx++;
        }
    }

    auto chars() const
    {
        return std::ranges::views::transform(data_, [](Cell const& c)
                                             { return c.as_char(); });
    }

    auto row(int idx)
    {
        return (rng::all(data_) | rng::chunk(9))[idx];
    }

    auto col(int idx)
    {
        return rng::all(data_) | rng::drop(idx) | rng::stride(9);
    }

    auto zone(int idx)
    {
        return (rng::all(data_) | rng::chunk(3) | rng::stride(3) | rng::chunk(3))[idx] | rng::join;
    }

    std::array<Cell, 81> data_;
};
