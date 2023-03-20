#pragma once
#include "ranges.h"

namespace ranges
{
#if USE_RANGEV3
    // Flattens a range of ranges by iterating the inner
    // ranges in round-robin fashion.
    template<class Rngs>
    class interleave_view : public ranges::view_facade<interleave_view<Rngs>> {
        friend ranges::range_access;
        std::vector<ranges::range_value_type_t<Rngs>> rngs_;
        struct cursor;
        cursor begin_cursor() {
            return {0, &rngs_, ranges::view::transform(rngs_, ranges::begin)};
        }
    public:
        interleave_view() = default;
        explicit interleave_view(Rngs rngs)
          : rngs_(std::move(rngs))
        {}
    };

    template<class Rngs>
    struct interleave_view<Rngs>::cursor  {
        std::size_t n_;
        std::vector<ranges::range_value_type_t<Rngs>> *rngs_;
        std::vector<ranges::iterator_t<ranges::range_value_type_t<Rngs>>> its_;
        decltype(auto) read() const {
            return *its_[n_];
        }
        void next() {
            if(0 == ((++n_) %= its_.size()))
                ranges::for_each(its_, [](auto& it){ ++it; });
        }
        bool equal(ranges::default_sentinel) const {
            return n_ == 0 && its_.end() != ranges::mismatch(its_, *rngs_,
                std::not_equal_to<>(), ranges::ident(), ranges::end).in1();
        }
        CONCEPT_REQUIRES(ranges::ForwardRange<ranges::range_value_type_t<Rngs>>())
        bool equal(cursor const& that) const {
            return n_ == that.n_ && its_ == that.its_;
        }
    };

    // In:  Range<Range<T>>
    // Out: Range<T>, flattened by walking the ranges
    //                round-robin fashion.
    inline auto interleave() {
        using namespace ranges;
        return make_pipeable([](auto&& rngs) {
            using Rngs = decltype(rngs);
            return interleave_view<view::all_t<Rngs>>(
                view::all(std::forward<Rngs>(rngs)));
        });
    }
    
#elif (0)
    template <rng::forward_range V>
        requires rng::view<V> && rng::sized_range<V> && rng::input_range<rng::range_reference_t<V>> && rng::sized_range<rng::range_reference_t<V>>
    struct interleave_view : rng::view_interface<interleave_view<V>>
    {
        struct iterator
        {
            using base_t = const V;
            using parent_t = const interleave_view;

            using outer_it = rng::iterator_t<base_t>;
            using inner_it = rng::iterator_t<rng::range_reference_t<base_t>>;

            using iterator_category = std::forward_iterator_tag;
            using value_type = std::invoke_result_t<(decltype(&iterator::current)>;
            using difference_type = std::ptrdiff_t;
            using pointer = void;
            using reference = void;

            constexpr iterator& operator++() { next(); return *this; }

            constexpr iterator() = default;
            constexpr iterator(parent_t* parent)
                : parent_(parent)
            {
                if (parent_ != nullptr)
                {
                    auto sizes = rng::transform_view(*base(), [](auto&& r){ return rng::size(r); });
                    max_idx_ = rng::min(sizes);
                }
            }

            constexpr value_type operator*() const
            {
                return current();
            }

            constexpr bool operator==(const iterator& rhs) const
            {
                if (parent_ != rhs.parent_)
                    return false;

                if (parent_ == nullptr)
                    return true;

                return idx_ == rhs.idx_;
            }

        private:
            constexpr base_t* base() const
            {
                if (parent_)
                    return &parent_->base_;
                return {};
            }

            constexpr void next(rng::range_difference_t<base_t> i)
            {
                idx_ = std::max(idx_ + i, max_idx_);
            }

            constexpr auto current() const
            {
                return rng::transform_view(*base(), 
                    [idx = idx_](auto&& inner) 
                    {  
                        auto it = std::next(inner.begin(), idx);
                        return *it;
                    });
            }

            rng::range_difference_t<base_t> idx_{};
            rng::range_difference_t<base_t> max_idx_{};
            parent_t* parent_{};
        };

        interleave_view() = default;
        constexpr interleave_view(V base) : base_(base) {}

        constexpr V base() const { return base_; }

        constexpr auto begin() const { return iterator(this); }
        constexpr auto end() const { return iterator(nullptr); }
        constexpr auto size() const { return ranges::size(base_); }

    private:
        V base_;
    };

    template <typename R>
    interleave_view(R&&) -> interleave_view<all_t<R>>;

    namespace detail
    {
        struct interleave_fn
        {
            template <typename R>
            constexpr auto operator()(R&& r) const
            {
                return interleave_view{std::forward<R>(r)};
            }

            constexpr auto operator()() const
            {
                return piped{[](auto&& r) { return interleave_view{std::forward<decltype(r)>(r)}; }};
            }
        };
    }

    inline namespace views
    {
        constexpr static detail::interleave_fn interleave{};
    }
#endif
}
