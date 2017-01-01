#ifndef BURST_FUNCTIONAL_NON_STRICT_HPP
#define BURST_FUNCTIONAL_NON_STRICT_HPP

#include <burst/type_traits/is_invokable.hpp>

#include <type_traits>
#include <utility>

namespace burst
{
    //!     Преобразование строгого порядка в нестрогий
    /*!
            Например, оператор "меньше" превращает в "меньше или равно".
     */
    template <typename BinaryPredicate>
    struct non_strict_fn
    {
        template <typename T, typename U>
        constexpr bool operator () (T && t, U && u) const
        {
            static_assert(is_invokable<BinaryPredicate(U, T)>::value, "");
            static_assert(std::is_convertible<std::result_of_t<BinaryPredicate(U, T)>, bool>::value, "");

            return not strict_order(std::forward<U>(u), std::forward<T>(t));
        }

        BinaryPredicate strict_order;
    };

    template <typename BinaryPredicate>
    auto non_strict (BinaryPredicate && strict_order)
        -> non_strict_fn<std::decay_t<BinaryPredicate>>
    {
        return {std::forward<BinaryPredicate>(strict_order)};
    }
}

#endif // BURST_FUNCTIONAL_NON_STRICT_HPP
