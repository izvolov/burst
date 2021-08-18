#ifndef BURST__RANGE__ADAPTOR__TO_VECTOR_HPP
#define BURST__RANGE__ADAPTOR__TO_VECTOR_HPP

#include <burst/range/adaptor/adaptor.hpp>
#include <burst/container/make_vector.hpp>

namespace burst
{
    struct make_vector_t
    {
        template <typename... Xs>
        constexpr auto operator () (Xs &&... xs) const
        {
            return make_vector(std::forward<Xs>(xs)...);
        }
    };

    /*!
        \brief
            Инструмент для конструирования вектора через конвейер

        \details
            Вызов

            \code{.cpp}
            range | to_vector
            \endcode

            эквивалентен вызову

            \code{.cpp}
            make_vector(range)
            \endcode

        \see make_vector
     */
    constexpr auto to_vector = make_adaptor_trigger(make_vector_t{});
} // namespace burst

#endif // BURST__RANGE__ADAPTOR__TO_VECTOR_HPP
