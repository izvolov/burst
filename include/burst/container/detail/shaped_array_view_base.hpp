#ifndef BURST__CONTAINER__DETAIL__SHAPED_ARRAY_VIEW_BASE_HPP
#define BURST__CONTAINER__DETAIL__SHAPED_ARRAY_VIEW_BASE_HPP

#include <array>
#include <cassert>
#include <cstddef>

namespace burst
{
    namespace detail
    {
        template <typename Value, std::size_t Dimension>
        struct shaped_array_view_base
        {
            Value * data;
            std::array<std::size_t, Dimension> shape;
        };

        template <typename Value, std::size_t Dimension, typename Integer>
        Value * slice (Value * data, std::array<std::size_t, Dimension> shape, Integer i)
        {
            assert(i >= 0);
            assert(static_cast<std::size_t>(i) < shape[0] / shape[1]);

            return data + static_cast<std::size_t>(i) * shape[1];
        }
    } // namespace detail
} // namespace burst

#endif // BURST__CONTAINER__DETAIL__SHAPED_ARRAY_VIEW_BASE_HPP
