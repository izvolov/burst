#ifndef BURST_RANGE_UNITE_HPP
#define BURST_RANGE_UNITE_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/union_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct unite_t
    {
        /*!
            \brief
                Функция для создания диапазона объединения

            \details
                Создаёт итератор объединения посредством пробрасывания аргументов этой функции в
                функцию `make_union_iterator`, а из этого итератора создаёт диапазон.

                Объединение диапазонов рассматривается в теоретико-множественном смысле, то есть в
                результирующем диапазоне все элементы, которые есть хотя бы в одном из входных
                диапазонов.

            \returns
                Диапазон, представляющий собой объединение нескольких (переданных в аргументах)
                диапазонов.

            \see make_union_iterator
            \see union_iterator
         */
        template <typename ... Args>
        auto operator () (Args && ... args) const
        {
            auto begin = make_union_iterator(std::forward<Args>(args)...);
            auto end = make_union_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto unite = unite_t{};
} // namespace burst

#endif // BURST_RANGE_UNITE_HPP
