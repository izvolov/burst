#ifndef BURST__TYPE_TRAITS__INVOKE_RESULT_HPP
#define BURST__TYPE_TRAITS__INVOKE_RESULT_HPP

#include <type_traits>

namespace burst
{
    template <typename F, typename... Args>
    struct invoke_result
    {
#ifdef __cpp_lib_is_invocable
        using type = std::invoke_result_t<F, Args...>;
#else
        using type = std::result_of_t<F(Args...)>;
#endif
    };

    template <typename F, typename... Args>
    using invoke_result_t = typename invoke_result<F, Args...>::type;
} // namespace burst

#endif // BURST__TYPE_TRAITS__INVOKE_RESULT_HPP

