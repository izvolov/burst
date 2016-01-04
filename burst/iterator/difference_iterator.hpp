#ifndef BURST_ITERATOR_DIFFERENCE_ITERATOR_HPP
#define BURST_ITERATOR_DIFFERENCE_ITERATOR_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/range/skip_to_lower_bound.hpp>

#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/concepts.hpp>

#include <functional>

namespace burst
{
    //!     Итератор разности множеств.
    /*!
            Предназначен для нахождения разности (в терминах теории множеств) между двумя
        диапазонами-множествами. Разность вычисляется "на лету", то есть без использования
        дополнительной памяти для хранения результирующего диапазона.
            Принимает на вход два упорядоченных диапазона и перемещается по тем элементам, которые
        есть в одном из них (уменьшаемом) и одновременно отсутствуют в другом (вычитаемом).
            Полученный в результате слияния диапазон изменяем. То есть из итератора можно не только
        прочитать значения, но можно и записать в него. В результате записи в итератор будет
        изменено значение в уменьшаемом.

        \tparam ForwardRange1
            Тип уменьшаемого диапазона. Он должен быть хотя бы однонаправленным, то есть
            удовлетворять требованиям понятия "Forward Range".
        \tparam ForwardRange2
            Тип вычитаемого диапазона. Он должен быть хотя бы однонаправленным, то есть
            удовлетворять требованиям понятия "Forward Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах входных диапазонов.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<>".

            Алгоритм работы.

        0. Инвариант
           Итератор разности всегда либо установлен на конец разности, либо первый элемент
           уменьшаемого строго меньше первого элемента вычитаемого.
           Концом разницы считается ситуация, когда уменьшаемый диапазон закончился.

        1. Поиск следующего элемента разности.
           а. Если вычитаемое пусто, то больше ничего делать не надо.
           б. Если и вычитаемое, и уменьшаемое непусты, и первые элементы уменьшаемого и
              вычитаемого равны, то нужно продвинуть их оба на один элемент вперёд и поддержать
              инвариант (п.0), после чего вернуться к п.1.
           в. Если оба диапазона непусты и первый элемент уменьшаемого строго меньше первого
              элемента вычитаемого, то текущий элемент разности найден.

        2. Каждый раз, когда нужно найти следующий элемент разности, надо продвинуть уменьшаемое на
           один элемент вперёд, после чего исполнить п.1.
     */
    template
    <
        typename ForwardRange1,
        typename ForwardRange2,
        typename Compare = std::less<>
    >
    class difference_iterator:
        public boost::iterator_facade
        <
            difference_iterator<ForwardRange1, ForwardRange2, Compare>,
            typename ForwardRange1::value_type,
            boost::forward_traversal_tag,
            typename ForwardRange1::reference
        >
    {
    private:
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<ForwardRange1>));
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<ForwardRange2>));
        using minuend_range_type = ForwardRange1;
        using subtrahend_range_type = ForwardRange2;
        using compare_type = Compare;

        using base_type =
            boost::iterator_facade
            <
                difference_iterator,
                typename minuend_range_type::value_type,
                boost::forward_traversal_tag,
                typename minuend_range_type::reference
            >;

    public:
        difference_iterator
                (
                    const minuend_range_type & minuend,
                    const subtrahend_range_type & subtrahend,
                    Compare compare = Compare()
                ):
            m_minuend(minuend),
            m_subtrahend(subtrahend),
            m_compare(compare)
        {
            BOOST_ASSERT(boost::algorithm::is_sorted(minuend, compare));
            BOOST_ASSERT(boost::algorithm::is_sorted(subtrahend, compare));

            maintain_invariant();
        }

        difference_iterator (const difference_iterator & begin, iterator::end_tag_t):
            m_minuend(begin.m_minuend.end(), begin.m_minuend.end()),
            m_subtrahend{},
            m_compare(begin.m_compare)
        {
        }

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            m_minuend.advance_begin(1);
            maintain_invariant();
        }

        //!     Выбросить все лишние элементы вычитаемого
        /*!
                Если уменьшаемое закончилось, то ничего делать не нужно.
                В противном случае надо отрезать кусок вычитаемого, предшествующий первому элементу
            уменьшаемого, то есть сделать так, чтобы первый элемент вычитаемого был, по крайней
            мере, не меньше первого элемента уменьшаемого.
         */
        void drop_subtrahend_head ()
        {
            if (not m_minuend.empty()
                && not m_subtrahend.empty()
                && m_compare(m_subtrahend.front(), m_minuend.front()))
            {
                skip_to_lower_bound(m_subtrahend, m_minuend.front(), m_compare);
            }
        }

        //!     Поддержать инвариант
        /*!
                Приводит диапазоны в состояние, когда первый элемент уменьшаемого строго меньше
            первого элемента вычитаемого (если вычитаемое не закончилось).
                Если в процессе продвижения уменьшаемый диапазон закончился, то итератор
            устанавливается на конец разности.
         */
        void maintain_invariant ()
        {
            while (not m_subtrahend.empty()
                && not m_minuend.empty()
                && not m_compare(m_minuend.front(), m_subtrahend.front()))
            {
                if (not m_compare(m_subtrahend.front(), m_minuend.front()))
                {
                    m_minuend.advance_begin(1);
                    m_subtrahend.advance_begin(1);
                }
                drop_subtrahend_head();
            }
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_minuend.front();
        }

        bool equal (const difference_iterator & that) const
        {
            return this->m_minuend == that.m_minuend;
        }

    private:
        minuend_range_type m_minuend;
        subtrahend_range_type m_subtrahend;
        compare_type m_compare;
    };

    //!     Функция для создания итератора разности с предикатом.
    /*!
            Принимает на вход два диапазона, разность которых нужно найти, и операцию, задающую
        отношение строгого порядка на элементах этих диапазонов.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первый элемент разности входных диапазонов.
     */
    template <typename ForwardRange1, typename ForwardRange2, typename Compare>
    auto make_difference_iterator (const ForwardRange1 & minuend, const ForwardRange2 & subtrahend, Compare compare)
    {
        return difference_iterator<ForwardRange1, ForwardRange2, Compare>(minuend, subtrahend, compare);
    }

    //!     Функция для создания итератора разности.
    /*!
            Принимает на вход два диапазона, разность которых нужно найти.
            Возвращает итератор на первый элемент разности входных диапазонов.
            Отношение порядка для элементов диапазонов выбирается по-умолчанию.
     */
    template <typename ForwardRange1, typename ForwardRange2>
    auto make_difference_iterator (const ForwardRange1 & minuend, const ForwardRange2 & subtrahend)
    {
        return difference_iterator<ForwardRange1, ForwardRange2>(minuend, subtrahend);
    }

    //!     Функция для создания итератора на конец разности с предикатом.
    /*!
            Принимает на вход итератор на начало разности и индикатор конца итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы разности
        закончились.
     */
    template <typename ForwardRange1, typename ForwardRange2, typename Compare>
    auto make_difference_iterator (const difference_iterator<ForwardRange1, ForwardRange2, Compare> & begin, iterator::end_tag_t)
    {
        return difference_iterator<ForwardRange1, ForwardRange2, Compare>(begin, iterator::end_tag);
    }
} // namespace burst

#endif // BURST_ITERATOR_DIFFERENCE_ITERATOR_HPP
