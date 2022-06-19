#pragma once
#include "ranges.h"

namespace ranges
{
    template<rng::input_range V, rng::forward_range Pattern>
        requires ranges::view<V> && ranges::input_range<ranges::range_reference_t<V>> && ranges::view<Pattern>
    struct join_with_view : public rng::view_interface<join_with_view<V, Pattern>>
    {
        using sentinel = std::default_sentinel_t;
        using value_type = ranges::range_value_t<ranges::range_reference_t<V>>;

        template <bool Const>
        struct iterator
        {
            using base_t = std::conditional_t<!Const, V, const V>;
            using parent_t = std::conditional_t<!Const, join_with_view, const join_with_view>;

            using iterator_category = typename rng::iterator_t<base_t>::iterator_category;

            using value_type = ranges::range_value_t<ranges::range_reference_t<base_t>>;
            using difference_type = std::ptrdiff_t;
            using pointer = void;
            using reference = rng::range_reference_t<rng::range_reference_t<base_t>>;

            constexpr iterator() = default;

            constexpr iterator(parent_t* parent, rng::iterator_t<base_t> it) 
                : parent_(parent)
                , outer_it_(it)
            {
                if (outer_it_ != rng::end(parent->base_))
                    inner_it_ = rng::begin(*outer_it_);
            }

            constexpr iterator& operator++() { next(); return *this; }
            constexpr iterator operator++(int) const { return iterator(); } // TODO
            //constexpr iterator& operator--() { prev(); return *this; }
            //constexpr iterator operator--(int) const { return iterator(); } // TODO

            constexpr value_type operator*() const
            {
                return current();
            }

            constexpr bool operator==(const iterator& rhs) const
            {
                if (parent_ != rhs.parent_)
                    return false;

                if (parent_ == nullptr)
                    return rhs.parent_ == nullptr || rhs.outer_it_ == rng::end(rhs.parent_->base_);

                if (outer_it_ == rng::end(parent_->base_))
                    return rhs.parent_ == nullptr || rhs.outer_it_ == rng::end(rhs.parent_->base_);

                return outer_it_ == rhs.outer_it_
                    && inner_it_ == rhs.inner_it_
                    && pattern_it_ == rhs.pattern_it_;
            }

            constexpr bool operator==(sentinel) const
                requires (!rng::common_range<V>)
            {
                return parent_ == nullptr || outer_it_ == rng::end(parent_->base_);
            }

        private:
            constexpr void next()
            {
                if (parent_ != nullptr && outer_it_ != rng::end(parent_->base_))
                {
                    if (inner_it_ != rng::end(*outer_it_))
                    {
                        ++inner_it_;
                        if (inner_it_ == rng::end(*outer_it_))
                        {
                            if (std::next(outer_it_) == rng::end(parent_->base_))
                            {
                                ++outer_it_;
                                return;
                            }
                            pattern_it_ = rng::begin(parent_->pattern_);
                        }
                    }
                    else 
                    {
                        ++pattern_it_;
                        if (pattern_it_ == rng::end(parent_->pattern_))
                        {
                            ++outer_it_;
                            if (outer_it_ == rng::end(parent_->base_))
                                return;
                            inner_it_ = rng::begin(*outer_it_);
                        }
                    }
                }
            }

            constexpr value_type current() const
            {
                if (parent_ != nullptr && outer_it_ != rng::end(parent_->base_))
                {
                    if (inner_it_ != rng::end(*outer_it_))
                        return *inner_it_;
                    else 
                        return *pattern_it_;
                }

                return {};
            }

            rng::iterator_t<base_t> outer_it_{};
            rng::iterator_t<rng::range_value_t<base_t>> inner_it_{};
            rng::iterator_t<const Pattern> pattern_it_{}; 
            const parent_t* parent_{};
        };

        constexpr join_with_view() = default;

        constexpr join_with_view(V v, Pattern p)
            : base_(std::move(v))
            , pattern_(std::move(p))
        {
        }

        template <rng::input_range R>
            requires std::constructible_from<V, all_t<R>> &&
                std::constructible_from<Pattern, rng::single_view<value_type>>
        constexpr join_with_view(R&& r, value_type e)
            : base_(rng::all(std::forward<R>(r)))
            , pattern_(rng::single_view<value_type>(std::move(e)))
        {
        }

        constexpr auto begin()
        {
            return iterator<false>(this, rng::begin(base_));
        }

        constexpr auto begin() const
        {
            return iterator<true>(this, rng::begin(base_));
        }

        constexpr auto end()
        {
            if constexpr (rng::common_range<V>)
            {
                return iterator<false>(this, rng::end(base_));
            }
            else
            {
                return sentinel();
            }
        }

        constexpr auto end() const
        {
            if constexpr (rng::common_range<V>)
            {
                return iterator<true>(this, rng::end(base_));
            }
            else
            {
                return sentinel();
            }
        }

    private:
        V base_{};
        Pattern pattern_{};
    };

    template <typename R, typename P>
    join_with_view(R&&, P&&) -> join_with_view<all_t<R>, all_t<P>>;

    template <typename R>
    join_with_view(R&&, rng::range_value_t<rng::range_reference_t<R>>) -> join_with_view<all_t<R>, single_view<rng::range_value_t<rng::range_reference_t<R>>>>;

    namespace detail
    {
        struct join_with_view_fn
        {
            template <typename P>
            constexpr auto operator()(P&& p) const
            {
                return piped{[p = std::forward<P>(p)](auto&& r) { return join_with_view(std::forward<decltype(r)>(r), std::move(p)); }};
            }

            template <typename R, typename P>
            constexpr auto operator()(R&& r, P&& p) const
            {
                return join_with_view(std::forward<R>(r), std::forward<P>(p));
            }
        };
    }

    inline namespace views
    {
        inline constexpr detail::join_with_view_fn join_with{};
    }
}