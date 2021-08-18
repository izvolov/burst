#ifndef BURST__TYPE_TRAITS__ALIGNED_VOLUME_HPP
#define BURST__TYPE_TRAITS__ALIGNED_VOLUME_HPP

#include <burst/integer/intceil.hpp>

#include <cstddef>

namespace burst
{
    namespace detail
    {
        template <std::size_t Total, typename ... Types>
        struct aligned_volume_impl;

        template <std::size_t Total, typename Head, typename ... Tail>
        struct aligned_volume_impl <Total, Head, Tail...>
        {
            static constexpr auto value =
                aligned_volume_impl
                <
                    intceil(Total, alignof(Head)) + sizeof(Head),
                    Tail...
                >
                ::value;
        };

        template <std::size_t Total>
        struct aligned_volume_impl <Total>
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
        static constexpr auto value = detail::aligned_volume_impl<0, Args...>::value;
    };

    template <typename ... Args>
    constexpr auto aligned_volume_v = aligned_volume<Args...>::value;
} // namespace burst

#endif // BURST__TYPE_TRAITS__ALIGNED_VOLUME_HPP
