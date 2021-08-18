#ifndef BURST__RANGE__ADAPTOR__TO_SET_HPP
#define BURST__RANGE__ADAPTOR__TO_SET_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/container/make_set.hpp>

namespace burst
{
    struct make_set_t
    {
        template <typename... Xs>
        constexpr auto operator () (Xs &&... xs) const
        {
            return make_set(std::forward<Xs>(xs)...);
        }
    };

    /*!
        \brief
            Инструмент для конструирования класса std::set через конвейер

        \details
            Вызовы

            \code{.cpp}
            range | to_set
            range | to_set(std::greater<>{})
            \endcode

            соответственно эквивалентны вызовам

            \code{.cpp}
            make_set(range)
            make_set(range, std::greater<>{})
            \endcode

        \see make_set
     */
    constexpr auto to_set = make_adaptor_trigger(make_set_t{});
} // namespace burst

#endif // BURST__RANGE__ADAPTOR__TO_SET_HPP
