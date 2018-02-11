#ifndef BURST_FUNCTIONAL_IDENTITY_HPP
#define BURST_FUNCTIONAL_IDENTITY_HPP

#include <utility>

namespace burst
{
    //!     Тождественное отображение
    /*!
            Возвращает то же самое значение, которое принимает на вход.
     */
    struct identity_fn
    {
        template <typename Type>
        constexpr decltype(auto) operator () (Type && value) const
        {
            return std::forward<Type>(value);
        }
    };
    constexpr auto identity = identity_fn{};
}

#endif // BURST_FUNCTIONAL_IDENTITY_HPP
