#pragma once
#include "ranges.h"
#include <cstdlib>
/* Taken from Eric Niebler's Calendar example.
   https://github.com/ericniebler/range-v3/blob/master/example/calendar.cpp
*/

namespace sudoku::views
{
#if USE_RANGEV3
    // In:  Range<T>
    // Out: Range<Range<T>>, where each inner range has $n$ elements.
    //                       The last range may have fewer.
    template<class Rng>
    class chunk_view : public ranges::view_interface<chunk_view<Rng>, Rng> {
        CONCEPT_ASSERT(ranges::ForwardRange<Rng>());
        ranges::range_difference_type_t<Rng> n_;
        friend ranges::range_access;
        class adaptor;
        adaptor begin_adaptor() {
            return adaptor{n_, ranges::end(this->base())};
        }
    public:
        chunk_view() = default;
        chunk_view(Rng rng, ranges::range_difference_type_t<Rng> n)
          : chunk_view::view_adaptor(std::move(rng)), n_(n)
        {}
    };

    template<class Rng>
    class chunk_view<Rng>::adaptor : public ranges::adaptor_base {
        ranges::range_difference_type_t<Rng> n_;
        ranges::sentinel_t<Rng> end_;
    public:
        adaptor() = default;
        adaptor(ranges::range_difference_type_t<Rng> n, ranges::sentinel_t<Rng> end)
          : n_(n), end_(end)
        {}
        auto read(ranges::iterator_t<Rng> it) const {
            return ranges::view::take(ranges::make_iterator_range(std::move(it), end_), n_);
        }
        void next(ranges::iterator_t<Rng> &it) {
            ranges::advance(it, n_, end_);
        }
        void prev() = delete;
        void distance_to() = delete;
    };

    // In:  Range<T>
    // Out: Range<Range<T>>, where each inner range has $n$ elements.
    //                       The last range may have fewer.
    inline auto chunk(std::size_t n) {
        using namespace ranges;
        return make_pipeable([=](auto&& rng) {
            using Rng = decltype(rng);
            return chunk_view<view::all_t<Rng>>{
                view::all(std::forward<Rng>(rng)),
                static_cast<ranges::range_difference_type_t<Rng>>(n)};
        });
    }
#endif

#if USE_NANORANGE
    template <typename V>
    struct chunk_view : ranges::view_interface<chunk_view<V>>
    {
        using diff_t = ranges::range_difference_t<V>;

        template <bool Const>
        struct iterator
        {
            using base_t = std::conditional_t<!Const, V, const V>;
            using base_it = ranges::iterator_t<base_t>;

            using iterator_category = ranges::random_access_iterator_tag;

            using value_type = ranges::subrange<base_it>;
            using difference_type = ranges::iter_difference_t<base_it>;
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
            constexpr iterator& operator-=(difference_type n) { i_ = prev(n); return *this; }
            constexpr iterator operator-(difference_type n) const { return iterator{base_, prev(n), n_}; }

            constexpr difference_type operator-(iterator const& rhs) const
            {
                auto q = std::div(i_ - rhs.i_, n_);
                return q.quot + !!q.rem;
            }

            constexpr value_type operator*() const
            { 
                return ranges::subrange(i_, next());
            }

            constexpr value_type operator[](difference_type n) const
            {
                return *(*this + n);
            }
            
            // makes nano::detail::detail::weakly_equality_comparable_with happy for common_ranges
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

            constexpr auto operator<=>(ranges::sentinel_t<base_t>) const 
            {
                done() ? std::strong_ordering::equal : std::strong_ordering::less;
            }

        private:
            constexpr base_it next(difference_type i = 1) const { return ranges::next(i_, i * n_, ranges::end(*base_)); }
            constexpr base_it prev(difference_type i = 1) const { return ranges::prev(i_, i * n_, ranges::begin(*base_)); }

            constexpr bool done() const { return base_ == nullptr || i_ == ranges::end(*base_); }

            base_t* base_ = nullptr;
            base_it i_;
            difference_type n_ = 0;
        };

        static_assert(ranges::view<V>);
        static_assert(ranges::input_range<V>);

        constexpr chunk_view() = default;

        constexpr chunk_view(V base, diff_t count)
            : base_(std::move(base))
            , count_(count)
        {
        }

        constexpr V base() const { return base_; }

        constexpr auto begin()
        {
            return iterator<false>{&base_, ranges::begin(base_), count_};
        }

        constexpr auto begin() const
        {
            return iterator<true>{&base_, ranges::cbegin(base_), count_};
        }

        constexpr auto end() 
        { 
            if constexpr (ranges::common_range<V>)
                return iterator<false>{&base_, ranges::end(base_), count_};
            else
                return ranges::end(base_);
        }

        constexpr auto end() const 
        { 
            if constexpr (ranges::common_range<V>)
                return iterator<true>{&base_, ranges::end(base_), count_};
            else
                return ranges::end(base_);
        }

    private:
        V base_;
        diff_t count_ = 1;
    };

    template <typename R>
    chunk_view(R&&, ranges::range_difference_t<R>) -> chunk_view<ranges::all_view<R>>;

    namespace detail
    {
        struct chunk_view_fn
        {
            template <typename C>
            constexpr auto operator()(C&& count) const
            {
                return ranges::detail::rao_proxy{
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
            constexpr auto operator()(R&& r, ranges::range_difference_t<R> count) const
            {
                return chunk_view(std::forward<R>(r), count);
            }

        };
    }

    NANO_INLINE_VAR(detail::chunk_view_fn, chunk);
#endif
}
