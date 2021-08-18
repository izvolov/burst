#ifndef BURST__TUPLE__MAKE_TUPLE_HPP
#define BURST__TUPLE__MAKE_TUPLE_HPP

#include <tuple>
#include <utility>

namespace burst
{
    //!     Создать кортеж из набора элементов
    /*!
            См. `std::make_tuple`.
            Отличается только тем, что это не функция, а функциональный объект, который удобно
        использовать в комбинации с другими функциональными объектами.
     */
    struct make_tuple_fn
    {
        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) const
        {
            return std::make_tuple(std::forward<Ts>(ts)...);
        }
    };

    constexpr auto make_tuple = make_tuple_fn{};
} // namespace burst

#endif // BURST__TUPLE__MAKE_TUPLE_HPP
