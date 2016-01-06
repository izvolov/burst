#ifndef BURST_RANGE_DETAIL_HAS_ADVANCE_BEGIN_HPP
#define BURST_RANGE_DETAIL_HAS_ADVANCE_BEGIN_HPP

#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename...>
        using void_t = void;

        template <typename, typename = void_t<>>
        struct has_advance_begin: std::false_type {};

        template <typename Range>
        struct has_advance_begin
            <
                Range,
                void_t<decltype(std::declval<Range &>().advance_begin(1))>
            >:
            std::true_type {};
    } // namespace detail
} // namespace burst

#endif // BURST_RANGE_DETAIL_HAS_ADVANCE_BEGIN_HPP
