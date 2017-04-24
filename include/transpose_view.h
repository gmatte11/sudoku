#pragma once
#include <range/v3/all.hpp>
#include "interleave_view.h"
/* Inspired from Eric Niebler's Calendar example.
   https://github.com/ericniebler/range-v3/blob/master/example/calendar.cpp
*/

namespace sudoku_view
{
    // In:  Range<Range<T>>
    // Out: Range<Range<T>>, transposing the rows and columns.
    auto transpose() {
        using namespace ranges;
        return make_pipeable([](auto&& rngs) {
            using Rngs = decltype(rngs);
            CONCEPT_ASSERT(ForwardRange<Rngs>());
            return view::single(std::forward<Rngs>(rngs) | interleave());
                
        });
    }
}