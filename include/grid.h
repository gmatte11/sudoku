#pragma once

#include "ranges.h"

#include <array>
#include <optional>

template <typename T>
struct Ref : public std::reference_wrapper<T>
{
    using base_t = std::reference_wrapper<T>;
    inline static T dummy_;

    Ref() : base_t(dummy_) {}

    using base_t::operator=;
};

class Grid
{
public:
    struct Cell
    {
        uint8_t idx_ = 0;
        uint8_t val_ = 0;
        bool fixed_ = false;

        void init(uint8_t val)
        {
            set(val);
            fixed_ = (val != 0);
        }

        void set(uint8_t val)
        {
            val_ = val;
        }

        char as_char() const { return (val_ != 0) ? val_ + '0' : '_'; }
        uint8_t val() const { return val_; }

        operator char() const { return as_char(); }

        bool operator==(Cell const& rhs) const { return idx_ == rhs.idx_; }
        auto operator<=>(Cell const& rhs) const { return idx_ <=> rhs.idx_; }
    };

    std::array<Cell, 81> data_;
    std::array<std::array<Ref<Cell>, 9>, 9> zones_;

    Grid()
    {
        constexpr std::array<int, 9> top_left_indexes = {0, 3, 6, 27, 30, 33, 54, 57, 60};

        for (int i = 0; i < 9; ++i)
        {
            const int top_left = top_left_indexes[i];
            auto& zone = zones_[i];

            zone[0] = data_[top_left + 0];
            zone[1] = data_[top_left + 1];
            zone[2] = data_[top_left + 2];
            zone[3] = data_[top_left + 9];
            zone[4] = data_[top_left + 10];
            zone[5] = data_[top_left + 11];
            zone[6] = data_[top_left + 18];
            zone[7] = data_[top_left + 19];
            zone[8] = data_[top_left + 20];
        }
    }

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

        constexpr std::array<int, 9> top_left_indexes = {0, 3, 6, 27, 30, 33, 54, 57, 60};

        for (int i = 0; i < 9; ++i)
        {
            const int top_left = top_left_indexes[i];
            auto& zone = zones_[i];

            zone[0] = data_[top_left +  0];
            zone[1] = data_[top_left +  1];
            zone[2] = data_[top_left +  2];
            zone[3] = data_[top_left +  9];
            zone[4] = data_[top_left + 10];
            zone[5] = data_[top_left + 11];
            zone[6] = data_[top_left + 18];
            zone[7] = data_[top_left + 19];
            zone[8] = data_[top_left + 20];
        }
    }

    std::span<Cell> cells() { return data_; }
    std::span<const Cell> cells() const { return data_; }

    auto chars() const
    {
        return views::transform(data_, [](Cell const& c)
                                             { return c.as_char(); });
    }

    auto row(int idx)
    {
        return (views::all(data_) | views::chunk(9))[idx];
    }

    auto col(int idx)
    {
        return views::all(data_) | views::drop(idx) | views::stride(9);
    }


    auto zone_of(Cell const& cell) const
        -> std::optional<decltype(views::all(Grid::zones_[0]))>
    {
        for (int i = 0; i < zones_.size(); ++i)
        {
            auto& zone = zones_[i];
            for (int j = 0; j < zone.size(); ++j)
            {
                Cell const& c = zone[j];
                if (c == cell)
                    return std::optional(views::all(zone));
            }
        }

        return {};
    }

    auto zone(int idx)
    {
        return views::all(zones_[idx]);
    }

    int next_idx(int from_idx) const
    {
        std::span<const Cell> tail(data_.begin() + (from_idx + 1), data_.end());

        auto next = ranges::find_if(tail, [](Cell const& c) { return !c.fixed_; });
        
        if (next == tail.end())
            return -1;

        return next->idx_;
    }

    int prev_idx(int from_idx) const
    {
        std::span<const Cell> head(data_.begin(), data_.begin() + from_idx);

        auto prev = ranges::find_if(head | views::reverse, [](Cell const& c) { return !c.fixed_; });

        if (prev == head.rend())
            return -1;

        return prev->idx_;
    }
};
