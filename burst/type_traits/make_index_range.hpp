#ifndef BURST_TYPE_TRAITS_MAKE_INDEX_RANGE_HPP
#define BURST_TYPE_TRAITS_MAKE_INDEX_RANGE_HPP

#include <cstddef>
#include <type_traits>

namespace burst
{
    template <typename IndexSequence, std::size_t Shift>
    struct shift_t;

    template <std::size_t ... Indices, std::size_t Shift>
    struct shift_t <std::index_sequence<Indices...>, Shift>
    {
        using type = std::index_sequence<Shift + Indices...>;
    };

    template <typename IndexSequence, std::size_t Shift>
    using shift = typename shift_t<IndexSequence, Shift>::type;

    template <std::size_t Begin, std::size_t End,
        typename = std::enable_if_t<(Begin <= End)>>
    using make_index_range = shift<std::make_index_sequence<End - Begin>, Begin>;
} // namespace burst

#endif // BURST_TYPE_TRAITS_MAKE_INDEX_RANGE_HPP
