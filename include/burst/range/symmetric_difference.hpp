#ifndef BURST__RANGE__SYMMETRIC_DIFFERENCE_HPP
#define BURST__RANGE__SYMMETRIC_DIFFERENCE_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/symmetric_difference_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct symmetric_difference_t
    {
        /*!
            \brief
                Функция для создания диапазона симметрической разности

            \details
                Создаёт итератор симметрической разности посредством пробрасывания аргументов этой
                функции в функцию `make_symmetric_difference_iterator`, а из этого итератора создаёт
                диапазон.

                Симметрическая разность диапазонов рассматривается в теоретико-множественном смысле,
                то есть в результирующем диапазоне будут те, и только те элементы, которые есть в
                нечётном количестве входных диапазонов.

            \returns
                Диапазон, представляющий собой симметрическую разность нескольких (переданных в
                аргументах) диапазонов.

            \see make_symmetric_difference_iterator
            \see symmetric_difference_iterator
         */
        template <typename ... Args>
        auto operator () (Args && ... args) const
        {
            auto begin = make_symmetric_difference_iterator(std::forward<Args>(args)...);
            auto end = make_symmetric_difference_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto symmetric_difference = symmetric_difference_t{};
}

#endif // BURST__RANGE__SYMMETRIC_DIFFERENCE_HPP
