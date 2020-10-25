#ifndef BURST_RANGE_ADAPTOR_TO_FORWARD_LIST_HPP
#define BURST_RANGE_ADAPTOR_TO_FORWARD_LIST_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/container/make_forward_list.hpp>

namespace burst
{
    struct make_forward_list_t
    {
        template <typename... Xs>
        constexpr auto operator () (Xs &&... xs) const
        {
            return make_forward_list(std::forward<Xs>(xs)...);
        }
    };

    /*!
        \brief
            Инструмент для конструирования односвязного списка через конвейер

        \details
            Вызов

            \code{.cpp}
            range | to_forward_list
            \endcode

            эквивалентен вызову

            \code{.cpp}
            make_forward_list(range)
            \endcode

        \see make_forward_list
     */
    constexpr auto to_forward_list = make_adaptor_trigger(make_forward_list_t{});
} // namespace burst

#endif // BURST_RANGE_ADAPTOR_TO_FORWARD_LIST_HPP
