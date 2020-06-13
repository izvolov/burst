#ifndef BURST_ITERATOR_SYMMETRIC_DIFFERENCE_ITERATOR_HPP
#define BURST_ITERATOR_SYMMETRIC_DIFFERENCE_ITERATOR_HPP

#include <burst/algorithm/select_min.hpp>
#include <burst/container/access/front.hpp>
#include <burst/functional/each.hpp>
#include <burst/iterator/detail/prevent_writing.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/type_traits/iterator_value.hpp>
#include <burst/type_traits/range_reference.hpp>
#include <burst/type_traits/range_value.hpp>

#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/concepts.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

namespace burst
{
    //!     Итератор симметрической разности
    /*!
            Симметрическая разность нескольких множеств — это множество, состоящее из элементов,
        которые присутствуют в нечётном количестве этих множеств.

            Предназначен для нахождения симметрической разности нескольких диапазонов одного типа
        "на лету", то есть без использования дополнительной памяти для хранения результирующего
        диапазона.
            Принимает на вход набор упорядоченных диапазонов и перемещается по тем элементам,
        которые в нечётном количестве этих диапазонов.
            Входные диапазоны рассматриваются как мультимножества.
            Результирующий диапазон неизменяем. То есть из итератора можно только прочитать
        значения, но нельзя записать. Такое ограничение обусловлено тем, что каждый выдаваемый
        итератором элемент может присутствовать в нескольких из входных диапазонов, а значит,
        нельзя однозначно выбрать из них какой-то один для записи нового значения.
            Между элементами результирующего диапазона сохраняется заданное отношение порядка.

        \tparam RandomAccessIterator
            Тип итератора принимаемого на вход внешнего диапазона. Он должен быть диапазоном
            произвольного доступа, то есть удовлетворять требованиям понятия "Random Access
            Iterator".
        \tparam Compare
            Двухместная операция, задающая отношение строгого порядка на элементах внутренних
            диапазонов. Если пользователем явно не указана операция, то, по-умолчанию, берётся
            отношение "меньше", задаваемое функциональным объектом "std::less<>".

            Алгоритм работы.

        1.  Инвариант
            Либо внешний диапазон пуст, либо на первых N местах (N — любое нечётное число) стоят
            диапазоны с наименьшими первыми элементами.
            Текущим элементом является первый элемент первого из внутренних диапазонов.
        2.  Поиск следующего элемента
            а.  Первые N диапазонов продвигаются на один элемент вперёд.
            б.  Каждый диапазон, который закончился, выбрасывается из рассмотрения. Когда все
                диапазоны закончились, элементов симметрической разности больше нет.
            в.  Восстановление инварианта.
     */
    template
    <
        typename RandomAccessIterator,
        typename Compare = std::less<>
    >
    class symmetric_difference_iterator:
        public boost::iterator_facade
        <
            symmetric_difference_iterator<RandomAccessIterator, Compare>,
            range_value_t<iterator_value_t<RandomAccessIterator>>,
            boost::single_pass_traversal_tag,
            detail::prevent_writing_t<range_reference_t<iterator_value_t<RandomAccessIterator>>>
        >
    {
    private:
        using outer_range_iterator = RandomAccessIterator;
        BOOST_CONCEPT_ASSERT((boost::RandomAccessIteratorConcept<outer_range_iterator>));

        using inner_range_type = iterator_value_t<outer_range_iterator>;
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<inner_range_type>));

        using compare_type = Compare;

        using base_type =
            boost::iterator_facade
            <
                symmetric_difference_iterator,
                range_value_t<inner_range_type>,
                boost::single_pass_traversal_tag,
                detail::prevent_writing_t<range_reference_t<inner_range_type>>
            >;

    public:
        symmetric_difference_iterator
            (
                outer_range_iterator first, outer_range_iterator last,
                Compare compare = Compare()
            ):
            m_begin(std::move(first)),
            m_end(std::move(last)),
            m_min_end(m_begin),
            m_compare(compare)
        {
            BOOST_ASSERT(std::all_of(m_begin, m_end,
                [this] (const auto & range)
                {
                    return boost::algorithm::is_sorted(range, m_compare);
                }));

            settle();
        }

        symmetric_difference_iterator
        (
            iterator::end_tag_t,
            const symmetric_difference_iterator & begin
        ):
            m_begin(begin.m_begin),
            m_end(begin.m_begin),
            m_min_end(begin.m_begin),
            m_compare(begin.m_compare)
        {
        }

        symmetric_difference_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void remove_empty_ranges ()
        {
            m_end = std::remove_if(m_begin, m_end, [] (const auto & r) {return r.empty();});
        }

        //!     Поддержать инвариант
        /*!
                Подробно инвариант описан в п.1 алгоритма работы.
         */
        void maintain_invariant ()
        {
            m_min_end = select_min(m_begin, m_end, each(front) | m_compare);
            while (m_min_end != m_begin && std::distance(m_begin, m_min_end) % 2 == 0)
            {
                std::for_each(m_begin, m_min_end, [] (auto & r) {r.advance_begin(1);});
                remove_empty_ranges();
                m_min_end = select_min(m_begin, m_end, each(front) | m_compare);
            }
        }

        void increment ()
        {
            faze();
            settle();
        }

        //!     Вывести диапазоны из равновесия
        /*!
                Приводит диапазоны в такое состояние, что предыдущий элемент уже недостижим, а
            следующий в общем случае ещё не достигнут.
                Продвигает диапазоны, содержащие на первом месте текущий элемент, на один элемент
            вперёд.
         */
        void faze ()
        {
            std::for_each(m_begin, m_min_end, [] (auto & r) {r.advance_begin(1);});
        }

        //!     Устаканить диапазоны на ближайшем элементе симметрической разности
        /*!
                Выбрасывает из рассмотрения пустые диапазоны и восстанавливает инвариант.
         */
        void settle ()
        {
            remove_empty_ranges();
            maintain_invariant();
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_begin->front();
        }

        bool equal (const symmetric_difference_iterator & that) const
        {
            assert(this->m_begin == that.m_begin);
            return std::equal(this->m_begin, this->m_end, that.m_begin, that.m_end);
        }

    private:
        outer_range_iterator m_begin;
        outer_range_iterator m_end;
        outer_range_iterator m_min_end; // Итератор на позицию за последним минимальным элементом.
        compare_type m_compare;
    };

    //!     Функция для создания итератора симметрической разности с предикатом.
    /*!
            Принимает на вход набор диапазонов, для которых нужно найти симметрическую разность,
        и операцию, задающую отношение строгого порядка на элементах этих диапазонов.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первый элемент симметрической разности входных диапазонов.
     */
    template <typename RandomAccessIterator, typename Compare>
    auto
        make_symmetric_difference_iterator
        (
            RandomAccessIterator first, RandomAccessIterator last,
            Compare compare
        )
    {
        return
            symmetric_difference_iterator<RandomAccessIterator, Compare>
            (
                std::move(first), std::move(last),
                compare
            );
    }

    template <typename RandomAccessRange, typename Compare>
    auto make_symmetric_difference_iterator (RandomAccessRange && ranges, Compare compare)
    {
        using std::begin;
        using std::end;
        return
            make_symmetric_difference_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges)),
                compare
            );
    }

    //!     Функция для создания итератора симметрической разности
    /*!
            Принимает на вход набор диапазонов, для которых нужно найти симметрическую разность.
            Возвращает итератор на первый элемент симметрической разности входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessIterator>
    auto make_symmetric_difference_iterator (RandomAccessIterator first, RandomAccessIterator last)
    {
        return
            symmetric_difference_iterator<RandomAccessIterator>(std::move(first), std::move(last));
    }

    template <typename RandomAccessRange>
    auto make_symmetric_difference_iterator (RandomAccessRange && ranges)
    {
        using std::begin;
        using std::end;
        return
            make_symmetric_difference_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges))
            );
    }

    //!     Функция для создания итератора на конец симметрической разности
    /*!
            Принимает на вход итератор на начало симметрической разности и индикатор конца
        итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы
        симметрической разности закончились.
     */
    template <typename RandomAccessIterator, typename Compare>
    auto
        make_symmetric_difference_iterator
        (
            iterator::end_tag_t,
            const symmetric_difference_iterator<RandomAccessIterator, Compare> & begin
        )
    {
        return
            symmetric_difference_iterator<RandomAccessIterator, Compare>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST_ITERATOR_SYMMETRIC_DIFFERENCE_ITERATOR_HPP
