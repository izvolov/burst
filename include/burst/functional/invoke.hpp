#ifndef BURST_FUNCTIONAL_INVOKE_HPP
#define BURST_FUNCTIONAL_INVOKE_HPP

#include <burst/type_traits/is_invokable.hpp>

#include <utility>

namespace burst
{
    //!     Вызов функции
    /*!
            Убогая замена функции `std::invoke` до выхода C++17.
            Единственное отличие в том, что это не функция, а функциональный объект, который можно
        удобно комбинировать с другими инструментами.
     */
    struct invoke_fn
    {
        template <typename F, typename ... As>
        constexpr decltype(auto) operator () (F && f, As && ... as) const
        {
            static_assert(is_invokable<F(As...)>{}, "");
            return std::forward<F>(f)(std::forward<As>(as)...);
        }
    };
    constexpr auto invoke = invoke_fn{};
} // namespace burst

#endif // BURST_FUNCTIONAL_INVOKE_HPP
