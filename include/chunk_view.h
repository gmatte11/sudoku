#pragma once
#include "ranges.h"

namespace ranges
{
    template <rng::view V>
    struct chunk_view : rng::view_interface<chunk_view<V>>
    {
        using diff_t = rng::range_difference_t<V>;

        template <bool Const>
        struct iterator
        {
            using base_t = std::conditional_t<!Const, V, const V>;
            using base_it = rng::iterator_t<base_t>;

            using iterator_category = std::random_access_iterator_tag;

            using value_type = rng::subrange<base_it>;
            using difference_type = std::iter_difference_t<base_it>;
            using pointer = void;
            using reference = value_type;

            constexpr iterator() = default;

            constexpr iterator(base_t* base, base_it i, difference_type n)
                : base_(base)
                , i_(i)
                , n_(n)
            {}


            constexpr iterator& operator++() { i_ = next(); return *this; }
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

            constexpr value_type operator*() const
            { 
                return rng::subrange(i_, next());
            }

            constexpr value_type operator[](difference_type n) const
            {
                return *(*this + n);
            }
            
            constexpr bool operator==(iterator const& rhs) const 
            {
                return i_ == rhs.i_;
            }

            constexpr auto operator<=>(iterator const& rhs) const
            {
                const bool lhs_done = done();
                const bool rhs_done = done();

                if (!lhs_done && !rhs_done)
                {
                    if (i_ < rhs.i_)
                        return std::strong_ordering::less;
                    else if (rhs.i_ < i_)
                        return std::strong_ordering::greater;
                }
                else if (!lhs_done)
                    return std::strong_ordering::less;
                else if (!rhs_done)
                    return std::strong_ordering::greater;

                return std::strong_ordering::equal;
            }

            constexpr auto operator<=>(rng::sentinel_t<base_t>) const 
            {
                done() ? std::strong_ordering::equal : std::strong_ordering::less;
            }

        private:
            constexpr base_it next(difference_type i = 1) const { return rng::next(i_, i * n_, rng::end(*base_)); }
            constexpr base_it prev(difference_type i = 1) const { return rng::prev(i_, i * n_, rng::begin(*base_)); }

            constexpr bool done() const { return base_ == nullptr || i_ == rng::end(*base_); }

            base_t* base_ = nullptr;
            base_it i_;
            difference_type n_ = 0;
        };

        constexpr chunk_view() = default;

        constexpr chunk_view(V base, diff_t count)
            : base_(std::move(base))
            , count_(count)
        {
        }

        constexpr V base() const { return base_; }

        constexpr auto begin()
        {
            return iterator<false>{&base_, rng::begin(base_), count_};
        }

        constexpr auto begin() const
        {
            return iterator<true>{&base_, rng::begin(base_), count_};
        }

        constexpr auto end() 
        { 
            if constexpr (rng::common_range<V>)
                return iterator<false>{&base_, rng::end(base_), count_};
            else
                return rng::end(base_);
        }

        constexpr auto end() const 
        { 
            if constexpr (rng::common_range<V>)
                return iterator<true>{&base_, rng::end(base_), count_};
            else
                return rng::end(base_);
        }

    private:
        V base_;
        diff_t count_ = 1;
    };

    template <rng::viewable_range R>
    chunk_view(R&&, rng::range_difference_t<R>) -> chunk_view<rng::all_t<R>>;

    namespace detail
    {
        struct chunk_view_fn
        {
            template <std::integral C>
            constexpr auto operator()(C count) const
            {
                return rng::detail::piped
                {
                    [count](auto&& r) mutable
                    {
                        return chunk_view(std::move(r), count);
                    }
                };
            }

            template <rng::viewable_range R, std::integral C>
            constexpr auto operator()(R&& r, C count) const
            {
                return chunk_view(std::move(r), count);
            }
        };
    }

    inline namespace views
    {
        inline constexpr detail::chunk_view_fn chunk{};
    }

#if USE_NANORANGE
    namespace detail
    {
        struct chunk_view_fn
        {
            template <typename C>
            constexpr auto operator()(C&& count) const
            {
                return rng::detail::rao_proxy{
                    [count](auto&& r) mutable
#ifndef NANO_MSVC_LAMBDA_PIPE_WORKAROUND
                        -> decltype(chunk_view{std::forward<decltype(r)>(r), std::move(count)})
#endif
                    {
                        return chunk_view(std::forward<decltype(r)>(r), std::move(count));
                    }
                };
            }

            template <typename R>
            constexpr auto operator()(R&& r, rng::range_difference_t<R> count) const
            {
                return chunk_view(std::forward<R>(r), count);
            }

        };
    }

    NANO_INLINE_VAR(detail::chunk_view_fn, chunk);
#endif
}
