#pragma once
#include <range/v3/all.hpp>
/* Taken from Eric Niebler's Calendar example.
   https://github.com/ericniebler/range-v3/blob/master/example/calendar.cpp
*/

namespace sudoku_view
{
    // In:  Range<T>
    // Out: Range<Range<T>>, where each inner range has $n$ elements.
    //                       The last range may have fewer.
    template<class Rng>
    class chunk_view : public ranges::view_adaptor<chunk_view<Rng>, Rng> {
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
    auto chunk(std::size_t n) {
        using namespace ranges;
        return make_pipeable([=](auto&& rng) {
            using Rng = decltype(rng);
            return chunk_view<view::all_t<Rng>>{
                view::all(std::forward<Rng>(rng)),
                static_cast<ranges::range_difference_type_t<Rng>>(n)};
        });
    }
}
