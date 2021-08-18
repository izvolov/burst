#ifndef BURST__TUPLE__FORWARD_AS_TUPLE_HPP
#define BURST__TUPLE__FORWARD_AS_TUPLE_HPP

#include <tuple>
#include <utility>

namespace burst
{
    //!     Пробросить элементы с помощью кортежа
    /*!
            См. `std::forward_as_tuple`.
            Отличается только тем, что это не функция, а функциональный объект, который удобно
        использовать в комбинации с другими функциональными объектами.
     */
    struct forward_as_tuple_fn
    {
        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) const
        {
            return std::forward_as_tuple(std::forward<Ts>(ts)...);
        }
    };

    constexpr auto forward_as_tuple = forward_as_tuple_fn{};
} // namespace burst

#endif // BURST__TUPLE__FORWARD_AS_TUPLE_HPP
