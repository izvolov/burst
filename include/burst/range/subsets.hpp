#ifndef BURST__RANGE__SUBSETS_HPP
#define BURST__RANGE__SUBSETS_HPP

#include <burst/iterator/subset_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    //!     Функция для создания диапазона подмножеств с предикатом.
    /*!
            Принимает на вход диапазон, подмножества которого нужно перебрать, и отношение порядка
        на элементах этого диапазона. Сам диапазон должен быть упорядочен в соответствии с этим
        отношением.
            Возвращает диапазон подмножеств исходного диапазона, то есть такой диапазон, каждый
        элемент которого — это, в свою очередь, тоже некий диапазон, пробежав по которому можно
        получить все элементы одного из подмножеств исходного диапазона.
     */
    template <typename ForwardRange, typename Compare>
    auto subsets (ForwardRange && range, Compare compare)
    {
        auto begin = make_subset_iterator(std::forward<ForwardRange>(range), compare);
        auto end = make_subset_iterator(iterator::end_tag, begin);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }

    //!     Функция для создания диапазона подмножеств.
    /*!
            Принимает на вход диапазон, подмножества которого нужно перебрать.
            Возвращает диапазон, состоящий из всех подмножеств исходного диапазона.
            Отношение порядка на элементах исходного диапазона выбирается по-умолчанию.
     */
    template <typename ForwardRange>
    auto subsets (ForwardRange && range)
    {
        auto begin = make_subset_iterator(std::forward<ForwardRange>(range));
        auto end = make_subset_iterator(iterator::end_tag, begin);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }
}

#endif // BURST__RANGE__SUBSETS_HPP
