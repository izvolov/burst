#ifndef BURST__FUNCTIONAL__INVERT_HPP
#define BURST__FUNCTIONAL__INVERT_HPP

#include <type_traits>
#include <utility>

namespace burst
{
    //!     Обратить двухместную операцию
    /*!
            Например, оператор "меньше" превращает в "больше".
     */
    template <typename BinaryFunction>
    struct invert_t
    {
        template <typename T, typename U>
        constexpr decltype(auto) operator () (T && t, U && u) const &
        {
            return f(std::forward<U>(u), std::forward<T>(t));
        }

        template <typename T, typename U>
        constexpr decltype(auto) operator () (T && t, U && u) &
        {
            return f(std::forward<U>(u), std::forward<T>(t));
        }

        template <typename T, typename U>
        constexpr decltype(auto) operator () (T && t, U && u) &&
        {
            return std::move(f)(std::forward<U>(u), std::forward<T>(t));
        }

        BinaryFunction f;
    };

    template <typename BinaryFunction>
    constexpr auto invert (BinaryFunction && f)
        -> invert_t<std::decay_t<BinaryFunction>>
    {
        return {std::forward<BinaryFunction>(f)};
    }
}

#endif // BURST__FUNCTIONAL__INVERT_HPP
