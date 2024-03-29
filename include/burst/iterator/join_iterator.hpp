#ifndef BURST__ITERATOR__JOIN_ITERATOR_HPP
#define BURST__ITERATOR__JOIN_ITERATOR_HPP

#include <burst/iterator/detail/join_iterator.hpp>
#include <burst/iterator/detail/uniform_range_tuple_please.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/own_as_range.hpp>
#include <burst/tuple/apply.hpp>
#include <burst/type_traits/iterator_value.hpp>
#include <burst/type_traits/range_iterator.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/minimum_category.hpp>

#include <iterator>
#include <tuple>
#include <utility>

namespace burst
{
    //!     Итератор склейки.
    /*!
            Предназначен для склейки нескольких диапазонов одного типа "на лету", то есть без
        использования дополнительной памяти для хранения результирующего диапазона.
            Принимает на вход набор диапазонов и перемещается по ним последовательно, то есть после
        последнего элемента i-го диапазона сразу переходит к первому элементу (i + 1)-го. Иначе
        говоря, в склеенном диапазоне вначале будут все элементы первой последовательности, затем
        все элементы второй, и т.д:

            join(R_1, ..., R_n) = R_1[1], ..., R_1[len_1], ..., R_n[1], ..., R_n[len_n]

        где R_i — i-й склеиваемый диапазон, len_i — длина i-го слеиваемого диапазона.
            Пустые диапазоны, естественно, игнорируются.

            Полученный в результате склейки диапазон изменяем. То есть из итератора можно не только
        прочитать значения, но можно и записать в него. В результате записи в итератор будет
        изменено значение в исходном хранилище.

        \tparam Range
            Тип принимаемого на вход диапазона.

            Категория итератора склейки.

            1. Итератор произвольного доступа.

               Если внешний диапазон и внутренние диапазоны одновременно являются диапазонами
               произвольного доступа, то итератор склейки будет итератором произвольного доступа,
               правда, с одной оговоркой: продвижение итератора на n шагов будет происходить не за
               O(1), а за O(|R|), где |R| — количество склеиваемых диапазонов.

            2. Однопроходный итератор.

               Если хотя бы один из типов диапазонов (внешний или внутренний) не является
               диапазоном произвольного доступа, то итератор склейки будет однопроходным
               итератором.

            Алгоритм работы

            Инвариант:
            Итератор склейки всегда установлен либо на элемент одного из непустых склеиваемых
        диапазонов, либо сразу за последним из склеиваемых диапазонов.
            Это условие выполняется сразу же при конструировании итератора склейки, а также после
        каждого продвижения итератора в любую сторону на любое количество позиций.
            Это справедливо и для случая произвольного доступа, и для однопроходного случая.

        I. Итератор произвольного доступа.
            1. Входной диапазон диапазонов копируется внутрь итератора.
            2. Заводятся два итератора: итератор во внешнем диапазоне и итератор в текущем
               внутреннем диапазоне.
            3. При продвижении итератора в какую-либо сторону изменяются только эти два итератора,
               а сам диапазон диапазонов остаётся нетронутым.

        II. Однопроходный итератор.
            1. Входной диапазон диапазонов копируется внутрь итератора.
            2. При продвижении итератора либо продвигается текущий внутренний диапазон, либо, если
               он закончился, внешний диапазон переходит к следующему внутреннему.
            3. Текущим элементом считается первый элемент текущего внутреннего диапазона.
     */
    template <typename Iterator>
    using join_iterator =
        detail::join_iterator_impl
        <
            Iterator,
            typename boost::iterators::minimum_category
            <
                typename boost::iterators::pure_iterator_traversal<Iterator>::type,
                typename boost::iterators::pure_iterator_traversal
                <
                    range_iterator_t<iterator_value_t<Iterator>>
                >
                ::type
            >
            ::type
        >;

    //!     Функция для создания итератора склейки.
    /*!
            Принимает на вход набор диапазонов, которые нужно склеить.
            Возвращает итератор на первый элемент склеенного списка, который является первым
        элементом первого из непустых входных диапазонов.
     */
    template <typename Iterator>
    auto make_join_iterator (Iterator first, Iterator last)
    {
        return join_iterator<Iterator>(std::move(first), std::move(last));
    }

    template <typename Range>
    auto make_join_iterator (Range && ranges)
    {
        using std::begin;
        using std::end;
        return
            make_join_iterator
            (
                begin(std::forward<Range>(ranges)),
                end(std::forward<Range>(ranges))
            );
    }

    /*!
        \brief
            Функция для создания итератора склейки из кортежа ссылок

        \details
            Создаёт итератор ленивой склейки нескольких диапазонов. Проход от созданного итератора
            до итератора-конца (см. перегрузку с `end_tag_t`) перечисляет все элементы непустых
            входных диапазонов, причём диапазоны перечисляются в порядке их размещения в кортеже
            `ranges`.

        \param ranges
            Кортеж ссылок на диапазоны, которые нужно склеить.

        \returns
            Итератор на первый элемент склеенного списка, который является первым элементом первого
            из непустых входных диапазонов.

        \see join_iterator
     */
    template <typename ... Ranges>
    auto make_join_iterator (std::tuple<Ranges &...> ranges)
    {
        auto common_ranges = detail::uniform_range_tuple_please(ranges);
        return make_join_iterator(own_as_range(burst::apply(make_range_vector, common_ranges)));
    }

    //!     Функция для создания итератора на конец склейки.
    /*!
            Принимает на вход итератор на начало склеенного диапазона и специальную метку,
        обозначающую, что нужно создать итератор на конец диапазона.
            Возвращает итератор на конец склеенного диапазона.
     */
    template <typename Iterator>
    auto make_join_iterator (iterator::end_tag_t, const join_iterator<Iterator> & begin)
    {
        return join_iterator<Iterator>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST__ITERATOR__JOIN_ITERATOR_HPP
