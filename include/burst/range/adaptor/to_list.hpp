#ifndef BURST__RANGE__ADAPTOR__TO_LIST_HPP
#define BURST__RANGE__ADAPTOR__TO_LIST_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/container/make_list.hpp>

namespace burst
{
    struct make_list_t
    {
        template <typename... Xs>
        constexpr auto operator () (Xs &&... xs) const
        {
            return make_list(std::forward<Xs>(xs)...);
        }
    };

    /*!
        \brief
            Инструмент для конструирования списка через конвейер

        \details
            Вызов

            \code{.cpp}
            range | to_list
            \endcode

            эквивалентен вызову

            \code{.cpp}
            make_list(range)
            \endcode

        \see make_list
     */
    constexpr auto to_list = make_adaptor_trigger(make_list_t{});
} // namespace burst

#endif // BURST__RANGE__ADAPTOR__TO_LIST_HPP
