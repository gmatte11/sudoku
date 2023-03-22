#pragma once
#include "ranges_util.h"

namespace ranges
{
    template <view V>
    struct stride_view : view_interface<stride_view<V>>
    {
        using diff_t = range_difference_t<V>;

        template <bool Const>
        struct iterator
        {
            using base_t = std::conditional_t<!Const, V, const V>;
            using base_it = iterator_t<base_t>;
            using base_st = sentinel_t<base_t>;

            using iterator_category = std::random_access_iterator_tag;

            using value_type = range_value_t<base_t>;
            using difference_type = std::iter_difference_t<base_it>;
            using pointer = void;
            using reference = range_reference_t<base_t>;

            constexpr iterator() = default;

            constexpr iterator(base_t* base, base_it i, difference_type n)
                : base_(base)
                , i_(i)
                , n_(n)
            {}

            constexpr iterator& operator++() {i_ = next(); return *this;} 
            constexpr iterator operator++(int) const { return iterator{base_, next(), n_}; }
            constexpr iterator& operator--() { i_ = prev(); return *this; }
            constexpr iterator operator--(int) const { return iterator{base_, prev(), n_}; }

            // TODO: negative numbers
            constexpr iterator& operator+=(difference_type n) { i_ = next(n); return *this; }
            constexpr iterator operator+(difference_type n) const { return iterator{base_, next(n), n_}; }
            friend constexpr iterator operator+(difference_type n, const iterator& it) { return it + n; } 
            constexpr iterator& operator-=(difference_type n) { i_ = prev(n); return *this; }
            constexpr iterator operator-(difference_type n) const { return iterator{base_, prev(n), n_}; }

            constexpr difference_type operator-(iterator const& rhs) const
            {
                auto q = std::div(i_ - rhs.i_, n_);
                return q.quot + !!q.rem;
            }

            constexpr reference operator*() const
            { 
                return *i_;
            }

            constexpr reference operator[](difference_type n) const
            {
                return *(*this + n);
            }
            
            constexpr bool operator==(iterator const& rhs) const 
            {
                return i_ == rhs.i_;
            }

            constexpr auto operator<=>(iterator const& rhs) const
            {
                return i_ <=> rhs.i_;
            }

        private:
            constexpr base_it next(difference_type n = 1) const { return ranges::next(i_, n * n_, ranges::end(*base_)); }
            constexpr base_it prev(difference_type n = 1) const { return ranges::prev(i_, n * n_, ranges::begin(*base_)); }

            base_t* base_ = nullptr;
            base_it i_;
            difference_type n_ = 0;
        };

        constexpr stride_view() = default;

        constexpr stride_view(V base, diff_t n)
            : base_(std::move(base))
            , stride_(n)
        {
        }

        constexpr V base() const { return base_; }

        constexpr auto begin()
        {
            return iterator<false>{&base_, ranges::begin(base_), stride_};
        }

        constexpr auto begin() const
        {
            return iterator<true>{&base_, ranges::begin(base_), stride_};
        }

        constexpr auto end()
        {
            if constexpr (common_range<V>)
                return iterator<false>{&base_, ranges::end(base_), stride_};
            else
                return end(base_);
        }

        constexpr auto end() const
        {
            if constexpr (common_range<V>)
                return iterator<true>{&base_, ranges::end(base_), stride_};
            else
                return end(base_);
        }

    private:
        V base_;
        diff_t stride_ = 1;
    };

    template <viewable_range R>
    stride_view(R&&, range_difference_t<R>) -> stride_view<views::all_t<R>>;

    namespace detail
    {
        struct stride_view_fn
        {
            template <std::integral I>
            constexpr auto operator()(I stride) const
            {
                return detail::piped
                {
                    [stride] (auto&& r) mutable
                    {
                        return stride_view(std::move(r), stride);
                    }
                };
            }
        
            template <viewable_range R, std::integral I>
            constexpr auto operator()(R&& r, I stride) const
            {
                return stride_view(std::move(r), stride);
            }
        };
    }

    namespace views
    {
        inline constexpr detail::stride_view_fn stride{};
    }
}

