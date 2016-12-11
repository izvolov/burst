#ifndef BURST_ITERATOR_DIFFERENCE_ITERATOR_HPP
#define BURST_ITERATOR_DIFFERENCE_ITERATOR_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/range/skip_to_lower_bound.hpp>

#include <boost/assert.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

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

        \tparam ForwardIterator1
            Тип итератора уменьшаемого диапазона. Он должен быть хотя бы однонаправленным, то есть
            удовлетворять требованиям понятия "Forward Iterator".
        \tparam ForwardIterator2
            Тип итератора вычитаемого диапазона. Он должен быть хотя бы однонаправленным, то есть
            удовлетворять требованиям понятия "Forward Iterator".
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
        typename ForwardIterator1,
        typename ForwardIterator2,
        typename Compare = std::less<>
    >
    class difference_iterator:
        public boost::iterator_facade
        <
            difference_iterator<ForwardIterator1, ForwardIterator2, Compare>,
            typename std::iterator_traits<ForwardIterator1>::value_type,
            boost::forward_traversal_tag,
            typename std::iterator_traits<ForwardIterator1>::reference
        >
    {
    private:
        BOOST_CONCEPT_ASSERT((boost::ForwardIteratorConcept<ForwardIterator1>));
        BOOST_CONCEPT_ASSERT((boost::ForwardIteratorConcept<ForwardIterator2>));
        using minuend_iterator = ForwardIterator1;
        using subtrahend_iterator = ForwardIterator2;
        using compare_type = Compare;

        using base_type =
            boost::iterator_facade
            <
                difference_iterator,
                typename std::iterator_traits<minuend_iterator>::value_type,
                boost::forward_traversal_tag,
                typename std::iterator_traits<minuend_iterator>::reference
            >;

    public:
        difference_iterator
            (
                minuend_iterator minuend_begin,
                minuend_iterator minuend_end,
                subtrahend_iterator subtrahend_begin,
                subtrahend_iterator subtrahend_end,
                Compare compare = Compare()
            ):
            m_minuend_begin(std::move(minuend_begin)),
            m_minuend_end(std::move(minuend_end)),
            m_subtrahend_begin(std::move(subtrahend_begin)),
            m_subtrahend_end(std::move(subtrahend_end)),
            m_compare(compare)
        {
            BOOST_ASSERT(std::is_sorted(m_minuend_begin, m_minuend_end, compare));
            BOOST_ASSERT(std::is_sorted(m_subtrahend_begin, m_subtrahend_end, compare));

            maintain_invariant();
        }

        difference_iterator (iterator::end_tag_t, const difference_iterator & begin):
            m_minuend_begin(begin.m_minuend_end),
            m_minuend_end(begin.m_minuend_end),
            m_subtrahend_begin{},
            m_subtrahend_end{},
            m_compare(begin.m_compare)
        {
        }

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            ++m_minuend_begin;
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
            if (m_minuend_begin != m_minuend_end
                && m_subtrahend_begin != m_subtrahend_end
                && m_compare(*m_subtrahend_begin, *m_minuend_begin))
            {
                auto subtrahend = boost::make_iterator_range(m_subtrahend_begin, m_subtrahend_end);
                skip_to_lower_bound(subtrahend, *m_minuend_begin, m_compare);
                m_subtrahend_begin = subtrahend.begin();
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
            while (m_subtrahend_begin != m_subtrahend_end
                && m_minuend_begin != m_minuend_end
                && not m_compare(*m_minuend_begin, *m_subtrahend_begin))
            {
                if (not m_compare(*m_subtrahend_begin, *m_minuend_begin))
                {
                    ++m_minuend_begin;
                    ++m_subtrahend_begin;
                }
                drop_subtrahend_head();
            }
        }

    private:
        typename base_type::reference dereference () const
        {
            return *m_minuend_begin;
        }

        bool equal (const difference_iterator & that) const
        {
            return this->m_minuend_begin == that.m_minuend_begin;
        }

    private:
        minuend_iterator m_minuend_begin;
        minuend_iterator m_minuend_end;
        subtrahend_iterator m_subtrahend_begin;
        subtrahend_iterator m_subtrahend_end;
        compare_type m_compare;
    };

    //!     Функция для создания итератора разности с предикатом.
    /*!
            Принимает на вход два диапазона, разность которых нужно найти, и операцию, задающую
        отношение строгого порядка на элементах этих диапазонов.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первый элемент разности входных диапазонов.
     */
    template <typename ForwardIterator1, typename ForwardIterator2, typename Compare>
    auto
        make_difference_iterator
        (
            ForwardIterator1 minuend_begin, ForwardIterator1 minuend_end,
            ForwardIterator2 subtrahend_begin, ForwardIterator2 subtrahend_end,
            Compare compare
        )
    {
        return
            difference_iterator<ForwardIterator1, ForwardIterator2, Compare>
            (
                std::move(minuend_begin),
                std::move(minuend_end),
                std::move(subtrahend_begin),
                std::move(subtrahend_end),
                compare
            );
    }

    template <typename ForwardRange1, typename ForwardRange2, typename Compare>
    auto
        make_difference_iterator
        (
            ForwardRange1 && minuend,
            ForwardRange2 && subtrahend,
            Compare compare
        )
    {
        return
            make_difference_iterator
            (
                std::begin(std::forward<ForwardRange1>(minuend)),
                std::end(std::forward<ForwardRange1>(minuend)),
                std::begin(std::forward<ForwardRange2>(subtrahend)),
                std::end(std::forward<ForwardRange2>(subtrahend)),
                compare
            );
    }

    //!     Функция для создания итератора разности.
    /*!
            Принимает на вход два диапазона, разность которых нужно найти.
            Возвращает итератор на первый элемент разности входных диапазонов.
            Отношение порядка для элементов диапазонов выбирается по-умолчанию.
     */
    template <typename ForwardIterator1, typename ForwardIterator2>
    auto
        make_difference_iterator
        (
            ForwardIterator1 minuend_begin, ForwardIterator1 minuend_end,
            ForwardIterator2 subtrahend_begin, ForwardIterator2 subtrahend_end
        )
    {
        return
            difference_iterator<ForwardIterator1, ForwardIterator2>
            (
                std::move(minuend_begin),
                std::move(minuend_end),
                std::move(subtrahend_begin),
                std::move(subtrahend_end)
            );
    }

    template <typename ForwardRange1, typename ForwardRange2,
        typename = std::enable_if_t
        <
            not std::is_same<std::decay_t<ForwardRange1>, iterator::end_tag_t>::value>
        >
    auto make_difference_iterator (ForwardRange1 && minuend, ForwardRange2 && subtrahend)
    {
        return
            make_difference_iterator
            (
                std::begin(std::forward<ForwardRange1>(minuend)),
                std::end(std::forward<ForwardRange1>(minuend)),
                std::begin(std::forward<ForwardRange2>(subtrahend)),
                std::end(std::forward<ForwardRange2>(subtrahend))
            );
    }

    //!     Функция для создания итератора на конец разности с предикатом.
    /*!
            Принимает на вход итератор на начало разности и индикатор конца итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы разности
        закончились.
     */
    template <typename ForwardIterator1, typename ForwardIterator2, typename Compare>
    auto
        make_difference_iterator
        (
            iterator::end_tag_t,
            const difference_iterator<ForwardIterator1, ForwardIterator2, Compare> & begin
        )
    {
        return
            difference_iterator<ForwardIterator1, ForwardIterator2, Compare>
            (
                iterator::end_tag,
                begin
            );
    }
} // namespace burst

#endif // BURST_ITERATOR_DIFFERENCE_ITERATOR_HPP
