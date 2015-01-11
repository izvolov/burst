#ifndef BURST_RANGE_SUBSET_HPP
#define BURST_RANGE_SUBSET_HPP

#include <type_traits>
#include <utility>

#include <boost/range/iterator_range.hpp>

#include <burst/iterator/subset_iterator.hpp>

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
    template <typename Range, typename Compare>
    boost::iterator_range
    <
        subset_iterator
        <
            typename std::decay<Range>::type,
            Compare
        >
    >
    subsets (Range && range, Compare compare)
    {
        return boost::make_iterator_range
        (
            make_subset_iterator(std::forward<Range>(range), compare),
            make_subset_iterator(std::forward<Range>(range), iterator::end_tag, compare)
        );
    }

    //!     Функция для создания диапазона подмножеств.
    /*!
            Принимает на вход диапазон, подмножества которого нужно перебрать.
            Возвращает диапазон, состоящий из всех подмножеств исходного диапазона.
            Отношение порядка на элементах исходного диапазона выбирается по-умолчанию.
     */
    template <typename Range>
    boost::iterator_range
    <
        subset_iterator
        <
            typename std::decay<Range>::type
        >
    >
    subsets (Range && range)
    {
        return boost::make_iterator_range
        (
            make_subset_iterator(std::forward<Range>(range)),
            make_subset_iterator(std::forward<Range>(range), iterator::end_tag)
        );
    }
}

#endif // BURST_RANGE_SUBSET_HPP
