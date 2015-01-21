#ifndef BURST_ITERATOR_INTERSECT_ITERATOR_HPP
#define BURST_ITERATOR_INTERSECT_ITERATOR_HPP

#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <burst/iterator/detail/front_value_compare.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/skip_to_lower_bound.hpp>

namespace burst
{
    //!     Итератор пересечения.
    /*!
            Предназначен для пересечения нескольких диапазонов одного типа "на лету", то есть без
        использования дополнительной памяти для хранения результирующего диапазона.
            Принимает на вход набор упорядоченных диапазонов и перемещается по тем элементам,
        которые есть в каждом из этих диапазонов.
            Полученный в результате пересечения диапазон неизменяем. То есть из итератора можно
        только прочитать значения, но нельзя записать. Такое ограничение обусловлено тем, что
        выдаваемые итератором элементы есть в каждом из входных диапазонов, а значит, нельзя
        однозначно выбрать из них какой-то один для записи нового значения.

        \tparam InputRange
            Тип принимаемого на вход диапазона. Он должен быть однопроходным, то есть удовлетворять
            требованиям понятия "Single Pass Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах диапазона
            InputRange.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<T>".

            Алгоритм работы.

        1. Диапазоны складываются в массив, в котором они упорядочиваются по своему первому элементу
           в том же отношении порядка, в котором упорядочены элементы в каждом из самих диапазонов.
        2. Сравниваются первый элемент первого диапазона и первый элемент последнего.
           а. Если они одинаковы, то это значит, что им равны и первые элементы всех диапазонов в
              середине массива (поскольку массив упорядочен). То есть пересечение найдено.
           б. Если первый меньше, то нужно так его продвинуть, чтобы первый его элемент был не
              меньше первого элемента последнего списка. Затем поместить первый диапазон на новое
              место в массиве так, чтобы поддержать заданное отношение порядка.
        3. Каждый раз, когда необходимо найти следующий элемент в пересечении, надо "вывести из
           равновесия" набор диапазонов, продвинув последний из них ровно на один элемент вперёд,
           после чего исполнить п.2.
     */
    template
    <
        typename InputRange,
        typename Compare = std::less<typename InputRange::value_type>
    >
    class intersect_iterator: public boost::iterator_facade
                                     <
                                         intersect_iterator<InputRange, Compare>,
                                         typename InputRange::value_type,
                                         boost::forward_traversal_tag,
                                         typename std::conditional
                                         <
                                             std::is_lvalue_reference<typename InputRange::reference>::value,
                                             typename std::remove_reference<typename InputRange::reference>::type const &,
                                             typename InputRange::reference
                                         >
                                         ::type
                                     >
    {
    private:
        typedef InputRange range_type;
        typedef Compare compare_type;

        typedef boost::iterator_facade
        <
            intersect_iterator,
            typename range_type::value_type,
            boost::forward_traversal_tag,
            typename std::conditional
            <
                std::is_lvalue_reference<typename range_type::reference>::value,
                typename std::remove_reference<typename range_type::reference>::type const &,
                typename range_type::reference
            >
            ::type
        >
        base_type;

    public:
        template <typename InputRange1>
        explicit intersect_iterator (const InputRange1 & ranges, Compare compare = Compare()):
            m_ranges(),
            m_compare(compare)
        {
            BOOST_STATIC_ASSERT(boost::is_same<typename InputRange1::value_type, range_type>::value);

            if (boost::algorithm::none_of(ranges, boost::bind(&range_type::empty, _1)))
            {
                BOOST_ASSERT(boost::algorithm::all_of(ranges.begin(), ranges.end(), boost::bind(&boost::algorithm::is_sorted<range_type, Compare>, _1, compare)));
                m_ranges.assign(ranges.begin(), ranges.end());
                boost::sort(m_ranges, detail::compare_by_front_value(m_compare));
                scroll_to_next_intersection();
            }
        }

        intersect_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            m_ranges.back().advance_begin(1);
            if (not m_ranges.back().empty())
            {
                scroll_to_next_intersection();
            }
            else
            {
                scroll_to_end();
            }
        }

        void scroll_to_next_intersection ()
        {
            while (m_compare(m_ranges.front().front(), m_ranges.back().front()))
            {
                skip_to_lower_bound(m_ranges.front(), m_ranges.back().front(), m_compare);
                if (not m_ranges.front().empty())
                {
                    auto second_range = ++m_ranges.begin();
                    auto new_position = std::lower_bound(second_range, m_ranges.end(), m_ranges.front(), detail::compare_by_front_value(m_compare));
                    std::rotate(m_ranges.begin(), second_range, new_position);
                }
                else
                {
                    scroll_to_end();
                    break;
                }
            }
        }

        void scroll_to_end ()
        {
            m_ranges.clear();
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_ranges.front().front();
        }

        bool equal (const intersect_iterator & that) const
        {
            return this->m_ranges == that.m_ranges;
        }

    private:
        std::vector<range_type> m_ranges;
        compare_type m_compare;
    };

    //!     Функция для создания итератора пересечения с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первое пересечение входных диапазонов.
     */
    template <typename RangeRange, typename Compare>
    intersect_iterator
    <
        typename RangeRange::value_type,
        Compare
    >
    make_intersect_iterator (const RangeRange & ranges, Compare compare)
    {
        return intersect_iterator<typename RangeRange::value_type, Compare>(ranges, compare);
    }

    //!     Функция для создания итератора пересечения.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь.
            Возвращает итератор на первое пересечение входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RangeRange>
    intersect_iterator
    <
        typename RangeRange::value_type
    >
    make_intersect_iterator (const RangeRange & ranges)
    {
        return intersect_iterator<typename RangeRange::value_type>(ranges);
    }

    //!     Функция для создания итератора на конец пересечения с предикатом.
    /*!
            Принимает на вход набор диапазонов, предикат и индикатор конца итератора.
            Набор диапазонов и предикат не используются, они нужны только для автоматического
        вывода типа итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что пересечения
        закончились.
     */
    template <typename RangeRange, typename Compare>
    intersect_iterator
    <
        typename RangeRange::value_type,
        Compare
    >
    make_intersect_iterator (const RangeRange &, Compare, iterator::end_tag_t)
    {
        return intersect_iterator<typename RangeRange::value_type, Compare>();
    }

    //!     Функция для создания итератора на конец пересечения.
    /*!
            Принимает на вход набор диапазонов, который не используется, а нужен только для
        автоматического вывода типа итератора.
            Возвращает итератор на конец последовательности пересечений.
            Отношение порядка берётся по-умолчанию.
     */
    template <typename RangeRange>
    intersect_iterator
    <
        typename RangeRange::value_type
    >
    make_intersect_iterator (const RangeRange &, iterator::end_tag_t)
    {
        return intersect_iterator<typename RangeRange::value_type>();
    }
} // namespace burst

#endif // BURST_ITERATOR_INTERSECT_ITERATOR_HPP
