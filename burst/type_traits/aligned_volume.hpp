#ifndef BURST_TYPE_TRAITS_ALIGNED_VOLUME_HPP
#define BURST_TYPE_TRAITS_ALIGNED_VOLUME_HPP

#include <burst/integer/intceil.hpp>

#include <algorithm>
#include <cstddef>

namespace burst
{
    namespace detail
    {
        template <std::size_t Total, std::size_t MaxAlignment, typename ... Tail>
        struct aligned_volume_impl;

        template <std::size_t Total, std::size_t MaxAlignment, typename Head, typename ... Tail>
        struct aligned_volume_impl <Total, MaxAlignment, Head, Tail...>
        {
            static constexpr auto value =
                aligned_volume_impl
                <
                    intceil(Total, alignof(Head)) + sizeof(Head),
                    std::max(MaxAlignment, alignof(Head)),
                    Tail...
                >
                ::value;
        };

        template <std::size_t Total, std::size_t MaxAlignment>
        struct aligned_volume_impl <Total, MaxAlignment>
        {
            static constexpr auto value = Total;
        };
    }

    //!     Объём выровненного хранилища для набора типов
    /*!
            Высчитывает, какой объём памяти (в байтах) необходим для того, чтобы уложить экземпляры
        заданных типов друг за другом в единый буфер с учётом выравнивания, причём именно в том
        порядке, в котором они перечислены во входных аргументах.
     */
    template <typename ... Args>
    struct aligned_volume
    {
        static constexpr auto value = detail::aligned_volume_impl<0, 0, Args...>::value;
    };
} // namespace burst

#endif // BURST_TYPE_TRAITS_ALIGNED_VOLUME_HPP
