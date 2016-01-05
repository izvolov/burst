#ifndef BURST_ITERATOR_MERGE_ITERATOR_HPP
#define BURST_ITERATOR_MERGE_ITERATOR_HPP

#include <burst/container/access/back.hpp>
#include <burst/iterator/detail/front_value_compare.hpp>
#include <burst/iterator/detail/invert_compare.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/range/concepts.hpp>
#include <boost/range/reference.hpp>
#include <boost/range/value_type.hpp>

#include <algorithm>
#include <functional>

namespace burst
{
    //!     Итератор слияния.
    /*!
            Предназначен для слияния нескольких диапазонов одного типа "на лету", то есть без
        использования дополнительной памяти для хранения результирующего диапазона.
            Принимает на вход набор упорядоченных диапазонов и перемещается по всем элементам
        этих диапазонов, сохраняя между ними заданное отношение порядка.
            Полученный в результате слияния диапазон изменяем. То есть из итератора можно не только
        прочитать значения, но можно и записать в него. В результате записи в итератор будет
        изменено значение в исходном хранилище.

        \tparam RandomAccessRange
            Тип принимаемого на вход внешнего диапазона. Он должен быть диапазоном произвольного
            доступа, то есть удовлетворять требованиям понятия "Random Access Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах внутренних
            диапазонов. Если пользователем явно не указана операция, то, по-умолчанию, берётся
            отношение "меньше", задаваемое функциональным объектом "std::less<>".

            Алгоритм работы.

        1. Внешний диапазон переупорядочивается в структуру "пирамида", в которой его элементы —
           внутренние диапазоны — сравниваются по первому элементу в том же отношении порядка, в
           котором упорядочены элементы в самих диапазонах.
        2. Каждый раз, когда требуется перейти к следующему элементу слияния, из пирамиды достаётся
           наименьший диапазон, он продвигается ровно на один элемент вперёд, а затем, если
           диапазон не стал пустым, кладётся обратно в пирамиду.
     */
    template
    <
        typename RandomAccessRange,
        typename Compare = std::less<>
    >
    class merge_iterator:
        public boost::iterator_facade
        <
            merge_iterator<RandomAccessRange, Compare>,
            typename boost::range_value<typename boost::range_value<RandomAccessRange>::type>::type,
            boost::single_pass_traversal_tag,
            typename boost::range_reference<typename boost::range_value<RandomAccessRange>::type>::type
        >
    {
    private:
        BOOST_CONCEPT_ASSERT((boost::RandomAccessRangeConcept<RandomAccessRange>));
        using outer_range_type = RandomAccessRange;

        using base_type =
            boost::iterator_facade
            <
                merge_iterator,
                typename boost::range_value<typename boost::range_value<outer_range_type>::type>::type,
                boost::single_pass_traversal_tag,
                typename boost::range_reference<typename boost::range_value<outer_range_type>::type>::type
            >;

    public:
        explicit merge_iterator (outer_range_type ranges, Compare compare = Compare()):
            m_range_heap(std::move(ranges)),
            m_heap_order(compare)
        {
            BOOST_ASSERT(boost::algorithm::all_of(m_range_heap,
                [& compare] (const auto & range)
                {
                    return boost::algorithm::is_sorted(range, compare);
                }));

            remove_empty_ranges();
            std::make_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
        }

        merge_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void remove_empty_ranges ()
        {
            m_range_heap.advance_end
            (
                -std::distance
                (
                    boost::remove_if(m_range_heap, [] (const auto & r) { return r.empty(); }),
                    std::end(m_range_heap)
                )
            );
        }

        void increment ()
        {
            std::pop_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
            auto & range = m_range_heap.back();

            range.advance_begin(1);
            if (not range.empty())
            {
                std::push_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
            }
            else
            {
                m_range_heap.advance_end(-1);
            }
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_range_heap.front().front();
        }

        bool equal (const merge_iterator & that) const
        {
            return this->m_range_heap == that.m_range_heap;
        }

    private:
        outer_range_type m_range_heap;

        // invert_comparison устраняет путаницу с обратным порядком в пирамиде при работе с
        // std::make(push, pop)_heap.
        using heap_order_type = detail::front_value_comparator<detail::invert_comparison<Compare>>;
        heap_order_type m_heap_order;
    };

    //!     Функция для создания итератора слияния с предикатом.
    /*!
            Принимает на вход диапазон диапазонов, которые нужно слить, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на наименьший относительно заданного отношения порядка элемент
        среди входных диапазонов.
     */
    template <typename RandomAccessRange, typename Compare>
    auto make_merge_iterator (RandomAccessRange ranges, Compare compare)
    {
        return merge_iterator<RandomAccessRange, Compare>(std::move(ranges), compare);
    }

    //!     Функция для создания итератора слияния.
    /*!
            Принимает на вход диапазон диапазонов, которые нужно слить.
            Возвращает итератор на наименьший элемент среди входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    auto make_merge_iterator (RandomAccessRange ranges)
    {
        return merge_iterator<RandomAccessRange>(std::move(ranges));
    }

    //!     Функция для создания итератора на конец слияния с предикатом.
    /*!
            Принимает на вход диапазон диапазонов, предикат и индикатор конца итератора.
            Диапазон диапазонов и предикат не используются, они нужны только для автоматического
        вывода типа итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы слияния
        закончились.
     */
    template <typename RandomAccessRange, typename Compare>
    auto make_merge_iterator (const RandomAccessRange &, Compare, iterator::end_tag_t)
    {
        return merge_iterator<RandomAccessRange, Compare>{};
    }

    //!     Функция для создания итератора на конец слияния.
    /*!
            Принимает на вход диапазон диапазонов, который не используется, а нужен только для
        автоматического вывода типа итератора.
            Возвращает итератор на конец слияния.
            Отношение порядка берётся по-умолчанию.
     */
    template <typename RandomAccessRange>
    auto make_merge_iterator (const RandomAccessRange &, iterator::end_tag_t)
    {
        return merge_iterator<RandomAccessRange>{};
    }
} // namespace burst

#endif // BURST_ITERATOR_MERGE_ITERATOR_HPP
