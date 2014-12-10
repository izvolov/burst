#ifndef THRUST_ITERATOR_JOIN_ITERATOR_HPP
#define THRUST_ITERATOR_JOIN_ITERATOR_HPP

#include <vector>

#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/bind.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <thrust/iterator/end_tag.hpp>

namespace thrust
{
    //!     Итератор склеивания.
    /*!
            Предназначен для склеивания нескольких диапазонов одного типа "на лету", то есть без
        использования дополнительной памяти для хранения результирующего диапазона.
            Принимает на вход набор диапазонов и перемещается по ним последовательно, то есть после
        последнего элемента i-го диапазона сразу переходит к первому элементу (i + 1)-го.
            Полученный в результате слияния диапазон изменяем. То есть из итератора можно не только
        прочитать значения, но можно и записать в него. В результате записи в итератор будет
        изменено значение в исходном хранилище.

        \tparam InputRange
            Тип принимаемого на вход диапазона. Он должен быть однопроходным, то есть удовлетворять
            требованиям понятия "Single Pass Range".

            Алгоритм работы.

        1. Входные диапазоны складываются в массив в обратном порядке.
        2. Каждый раз, когда требуется перейти к новому элементу, последний диапазон в массиве
           (первый в исходном наборе) продвигается ровно на один элемент вперёд.
           а. Если диапазон опустел, то он выбрасывается из массива.
           б. Если не опустел, то переход завершён.
        3. Пустой массив диапазонов означает, что пройдены все элементы всех входных диапазонов.
     */
    template <typename InputRange>
    class join_iterator: public boost::iterator_facade
                                <
                                    join_iterator<InputRange>,
                                    typename InputRange::value_type,
                                    boost::forward_traversal_tag,
                                    typename InputRange::reference
                                >
    {
    private:
        typedef InputRange range_type;

        typedef boost::iterator_facade
        <
            join_iterator,
            typename range_type::value_type,
            boost::forward_traversal_tag,
            typename range_type::reference
        >
        base_type;

    public:
        template <typename BidirectionalRange>
        explicit join_iterator (const BidirectionalRange & ranges):
            m_ranges()
        {
            BOOST_STATIC_ASSERT(boost::is_same<typename BidirectionalRange::value_type, range_type>::value);
            boost::algorithm::copy_if(boost::adaptors::reverse(ranges), std::back_inserter(m_ranges), not boost::bind(&range_type::empty, _1));
        }

        join_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            m_ranges.back().advance_begin(1);
            if (m_ranges.back().empty())
            {
                m_ranges.pop_back();
            }
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_ranges.back().front();
        }

        bool equal (const join_iterator & that) const
        {
            return this->m_ranges == that.m_ranges;
        }

    private:
        std::vector<range_type> m_ranges;
    };

    //!     Функция для создания итератора склеивания.
    /*!
            Принимает на вход набор диапазонов, которые нужно склеить.
            Возвращает итератор на первый элемент склеенного списка, который является первым
        элементом первого из входных диапазонов.
     */
    template <typename RangeRange>
    join_iterator<typename RangeRange::value_type> make_join_iterator (const RangeRange & ranges)
    {
        return join_iterator<typename RangeRange::value_type>(ranges);
    }

    //!     Функция для создания итератора на конец склеивания.
    /*!
            Принимает на вход набор диапазонов, который не используется, а нужен только для
        автоматического вывода типа итератора.
            Возвращает итератор на конец склеенного списка.
     */
    template <typename RangeRange>
    join_iterator<typename RangeRange::value_type> make_join_iterator (const RangeRange &, iterator::end_tag_t)
    {
        return join_iterator<typename RangeRange::value_type>();
    }
} // namespace thrust

#endif // THRUST_ITERATOR_JOIN_ITERATOR_HPP
