module;
#include "ranges_util.h"
#include "chunk_view.h"
#include "join_with_view.h"
#include "stride_view.h"

export module sudoku.ranges;

export namespace ranges
{
    using namespace std::ranges;
    using ranges::chunk_view;
    using ranges::join_with_view;
    using ranges::stride_view;

    export namespace views
    {
        using namespace std::ranges::views;

        using ranges::views::chunk;
        using ranges::views::join_with;    
        using ranges::views::stride;    
    }
}

export namespace views
{
    using namespace ranges::views;
}

export using ranges::detail::operator|;