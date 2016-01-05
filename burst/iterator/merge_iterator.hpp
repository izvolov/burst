#ifndef BURST_ITERATOR_MERGE_ITERATOR_HPP
#define BURST_ITERATOR_MERGE_ITERATOR_HPP

#include <burst/iterator/detail/front_value_compare.hpp>
#include <burst/iterator/detail/invert_compare.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/concepts.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <algorithm>
#include <functional>
#include <vector>

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

        \tparam ForwardRange
            Тип принимаемого на вход диапазона. Он должен быть однонаправленным, то есть
            удовлетворять требованиям понятия "Forward Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах входных диапазонов.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<T>".

            Алгоритм работы.

        1. Диапазоны складываются в структуру "пирамида", в которой они сравниваются по первому
           элементу в том же отношении порядка, в котором упорядочены элементы в самих диапазонах.
        2. Каждый раз, когда требуется перейти к следующему элементу слияния, из пирамиды достаётся
           наименьший диапазон, он продвигается ровно на один элемент вперёд, а затем, если
           диапазон не стал пустым, кладётся обратно в пирамиду.
     */
    template
    <
        typename ForwardRange,
        typename Compare = std::less<>
    >
    class merge_iterator:
        public boost::iterator_facade
        <
            merge_iterator<ForwardRange, Compare>,
            typename ForwardRange::value_type,
            boost::forward_traversal_tag,
            typename ForwardRange::reference
        >
    {
    private:
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<ForwardRange>));
        using range_type = ForwardRange;

        using base_type =
            boost::iterator_facade
            <
                merge_iterator,
                typename range_type::value_type,
                boost::forward_traversal_tag,
                typename range_type::reference
            >;

    public:
        template <typename RandomAccessRange>
        explicit merge_iterator (const RandomAccessRange & ranges, Compare compare = Compare()):
            m_range_heap(),
            m_heap_order(compare)
        {
            BOOST_STATIC_ASSERT(boost::is_same<typename RandomAccessRange::value_type, range_type>::value);
            BOOST_ASSERT(boost::algorithm::all_of(ranges,
                [& compare] (const auto & range)
                {
                    return boost::algorithm::is_sorted(range, compare);
                }));

            m_range_heap.reserve(ranges.size());
            boost::algorithm::copy_if(ranges, std::back_inserter(m_range_heap),
                [] (const auto & range)
                {
                    return not range.empty();
                });
            std::make_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
        }

        merge_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            std::pop_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
            range_type & range = m_range_heap.back();

            range.advance_begin(1);
            if (not range.empty())
            {
                std::push_heap(m_range_heap.begin(), m_range_heap.end(), m_heap_order);
            }
            else
            {
                m_range_heap.pop_back();
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
        std::vector<range_type> m_range_heap;

        // invert_comparison устраняет путаницу с обратным порядком в пирамиде при работе с
        // std::make(push, pop)_heap.
        using heap_order_type = detail::front_value_comparator<detail::invert_comparison<Compare>>;
        heap_order_type m_heap_order;
    };

    //!     Функция для создания итератора слияния с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно слить, и операцию, задающую отношение
        строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на наименьший относительно заданного отношения порядка элемент
        среди входных диапазонов.
     */
    template <typename RandomAccessRange, typename Compare>
    merge_iterator
    <
        typename RandomAccessRange::value_type,
        Compare
    >
    make_merge_iterator (const RandomAccessRange & ranges, Compare compare)
    {
        return merge_iterator<typename RandomAccessRange::value_type, Compare>(ranges, compare);
    }

    //!     Функция для создания итератора слияния.
    /*!
            Принимает на вход набор диапазонов, которые нужно слить.
            Возвращает итератор на наименьший элемент среди входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    merge_iterator
    <
        typename RandomAccessRange::value_type
    >
    make_merge_iterator (const RandomAccessRange & ranges)
    {
        return merge_iterator<typename RandomAccessRange::value_type>(ranges);
    }

    //!     Функция для создания итератора на конец слияния с предикатом.
    /*!
            Принимает на вход набор диапазонов, предикат и индикатор конца итератора.
            Набор диапазонов и предикат не используются, они нужны только для автоматического
        вывода типа итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы слияния
        закончились.
     */
    template <typename RandomAccessRange, typename Compare>
    merge_iterator
    <
        typename RandomAccessRange::value_type,
        Compare
    >
    make_merge_iterator (const RandomAccessRange &, Compare, iterator::end_tag_t)
    {
        return merge_iterator<typename RandomAccessRange::value_type, Compare>();
    }

    //!     Функция для создания итератора на конец слияния.
    /*!
            Принимает на вход набор диапазонов, который не используется, а нужен только для
        автоматического вывода типа итератора.
            Возвращает итератор на конец слияния.
            Отношение порядка берётся по-умолчанию.
     */
    template <typename RandomAccessRange>
    merge_iterator
    <
        typename RandomAccessRange::value_type
    >
    make_merge_iterator (const RandomAccessRange &, iterator::end_tag_t)
    {
        return merge_iterator<typename RandomAccessRange::value_type>();
    }
} // namespace burst

#endif // BURST_ITERATOR_MERGE_ITERATOR_HPP
