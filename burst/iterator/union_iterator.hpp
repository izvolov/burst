#ifndef BURST_ITERATOR_UNION_ITERATOR_HPP
#define BURST_ITERATOR_UNION_ITERATOR_HPP

#include <burst/iterator/detail/front_value_compare.hpp>
#include <burst/iterator/detail/range_range.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/range/algorithm/upper_bound.hpp>
#include <boost/range/concepts.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace burst
{
    //!     Итератор объединения.
    /*!
            Предназначен для объединения нескольких диапазонов одного типа "на лету", то есть без
        использования дополнительной памяти для хранения результирующего диапазона.
            Принимает на вход набор упорядоченных диапазонов и перемещается по элементам этих
        диапазонов так, что каждый элемент исходных диапазонов встречается столько раз, сколько он
        встретился в диапазоне, в котором он встречается чаще других. Проще говоря, объединяемые
        диапазоны рассматриваются как мультимножества.
            Между элементами результирующего диапазона сохраняется заданное отношение порядка.
            Полученный в результате объединения диапазон неизменяем. То есть из итератора можно
        только прочитать значения, но нельзя записать. Такое ограничение обусловлено тем, что
        каждый выдаваемый итератором элемент может присутствовать в нескольких из входных
        диапазонов, а значит, нельзя однозначно выбрать из них какой-то один для записи нового
        значения.

        \tparam RandomAccessRange
            Тип принимаемого на вход внешнего диапазона. Он должен быть диапазоном произвольного
            доступа, то есть удовлетворять требованиям понятия "Random Access Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах внутренних
            диапазонов.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<>".

            Алгоритм работы.

        1. Внешний диапазон переупорядочивается так, что его элементы (внутренние диапазоны)
           упорядочены по первому элементу в заданном отношении порядка.
           В каждый момент времени первый элемент первого внутреннего диапазона — текущий элемент
           объединения.
        2. Чтобы найти следующий элемент объединения, нужно продвинуть на один элемент вперёд все
           внутренние диапазоны, у которых первый элемент совпадает с текущим элементом
           объединения, а затем заново отсортировать внешний диапазон.
           Если в результате продвижения какой-либо из внутренних диапазонов опустел, он
           выбрасывается.
        3. Когда все внутренние диапазоны опустели, объединение закончено.
     */
    template
    <
        typename RandomAccessRange,
        typename Compare = std::less<>
    >
    class union_iterator:
        public boost::iterator_facade
        <
            union_iterator<RandomAccessRange, Compare>,
            detail::range_range_value_t<RandomAccessRange>,
            boost::single_pass_traversal_tag,
            typename std::conditional
            <
                std::is_lvalue_reference<detail::range_range_reference_t<RandomAccessRange>>::value,
                std::remove_reference_t<detail::range_range_reference_t<RandomAccessRange>> const &,
                detail::range_range_reference_t<RandomAccessRange>
            >
            ::type
        >
    {
    public:
        using outer_range_type = RandomAccessRange;
        BOOST_CONCEPT_ASSERT((boost::RandomAccessRangeConcept<outer_range_type>));

        using inner_range_type = typename boost::range_value<outer_range_type>::type;
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<inner_range_type>));

        using compare_type = Compare;

        using base_type =
            boost::iterator_facade
            <
                union_iterator,
                detail::range_range_value_t<outer_range_type>,
                boost::single_pass_traversal_tag,
                typename std::conditional
                <
                    std::is_lvalue_reference<detail::range_range_reference_t<outer_range_type>>::value,
                    std::remove_reference_t<detail::range_range_reference_t<outer_range_type>> const &,
                    detail::range_range_reference_t<outer_range_type>
                >
                ::type
            >;

    public:
        explicit union_iterator (outer_range_type ranges, compare_type compare = compare_type()):
            m_ranges(std::move(ranges)),
            m_compare(compare)
        {
            BOOST_ASSERT(boost::algorithm::all_of(m_ranges,
                [this] (const auto & range)
                {
                    return boost::algorithm::is_sorted(range, m_compare);
                }));

            maintain_invariant();
        }

        union_iterator (const union_iterator & begin, iterator::end_tag_t):
            m_ranges(std::begin(begin.m_ranges), std::begin(begin.m_ranges)),
            m_compare(begin.m_compare)
        {
        }

        union_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void remove_empty_ranges ()
        {
            m_ranges.advance_end
            (
                -std::distance
                (
                    boost::remove_if(m_ranges, [] (const auto & r) {return r.empty();}),
                    std::end(m_ranges)
                )
            );
        }

        void maintain_invariant ()
        {
            remove_empty_ranges();
            std::sort(m_ranges.begin(), m_ranges.end(), detail::compare_by_front_value(m_compare));
        }

        void increment ()
        {
            auto current_union_end = boost::upper_bound(m_ranges, m_ranges.front(), detail::compare_by_front_value(m_compare));
            std::for_each(m_ranges.begin(), current_union_end,
                [] (auto & range)
                {
                    range.advance_begin(1);
                });

            maintain_invariant();
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_ranges.front().front();
        }

        bool equal (const union_iterator & that) const
        {
            return this->m_ranges == that.m_ranges;
        }

    private:
        outer_range_type m_ranges;
        compare_type m_compare;

    };

    //!     Функция для создания итератора объединения с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно объединить, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первый элемент объединения входных диапазонов.
     */
    template <typename RandomAccessRange, typename Compare>
    auto make_union_iterator (RandomAccessRange ranges, Compare compare)
    {
        return union_iterator<RandomAccessRange, Compare>(std::move(ranges), compare);
    }

    //!     Функция для создания итератора объединения.
    /*!
            Принимает на вход набор диапазонов, которые нужно объединить.
            Возвращает итератор на первый элемент объединения входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    auto make_union_iterator (RandomAccessRange ranges)
    {
        return union_iterator<RandomAccessRange>(std::move(ranges));
    }

    //!     Функция для создания итератора на конец объединения с предикатом.
    /*!
            Принимает на вход итератор на начало объединения и индикатор конца итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы
        объединения закончились.
     */
    template <typename RandomAccessRange, typename Compare>
    auto
        make_union_iterator
        (
            const union_iterator<RandomAccessRange, Compare> & begin,
            iterator::end_tag_t
        )
    {
        return union_iterator<RandomAccessRange, Compare>(begin, iterator::end_tag);
    }
} // namespace burst

#endif // BURST_ITERATOR_UNION_ITERATOR_HPP
