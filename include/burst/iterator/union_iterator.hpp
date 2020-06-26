#ifndef BURST_ITERATOR_UNION_ITERATOR_HPP
#define BURST_ITERATOR_UNION_ITERATOR_HPP

#include <burst/container/access/front.hpp>
#include <burst/functional/each.hpp>
#include <burst/iterator/detail/prevent_writing.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/own_as_range.hpp>
#include <burst/tuple/apply.hpp>
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

        \tparam RandomAccessIterator
            Тип итератора принимаемого на вход внешнего диапазона. Он должен быть диапазоном
            произвольного доступа, то есть удовлетворять требованиям понятия "Random Access
            Iterator".
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
        typename RandomAccessIterator,
        typename Compare = std::less<>
    >
    class union_iterator:
        public boost::iterator_facade
        <
            union_iterator<RandomAccessIterator, Compare>,
            range_value_t<iterator_value_t<RandomAccessIterator>>,
            boost::single_pass_traversal_tag,
            detail::prevent_writing_t<range_reference_t<iterator_value_t<RandomAccessIterator>>>
        >
    {
    public:
        using outer_range_iterator = RandomAccessIterator;
        BOOST_CONCEPT_ASSERT((boost::RandomAccessIteratorConcept<outer_range_iterator>));

        using inner_range_type = iterator_value_t<outer_range_iterator>;
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<inner_range_type>));

        using compare_type = Compare;

        using base_type =
            boost::iterator_facade
            <
                union_iterator,
                range_value_t<inner_range_type>,
                boost::single_pass_traversal_tag,
                detail::prevent_writing_t<range_reference_t<inner_range_type>>
            >;

    public:
        union_iterator
            (
                outer_range_iterator first, outer_range_iterator last,
                compare_type compare = compare_type()
            ):
            m_begin(std::move(first)),
            m_end(std::move(last)),
            m_compare(compare)
        {
            BOOST_ASSERT(std::all_of(m_begin, m_end,
                [this] (const auto & range)
                {
                    return boost::algorithm::is_sorted(range, m_compare);
                }));

            maintain_invariant();
        }

        union_iterator (iterator::end_tag_t, const union_iterator & begin):
            m_begin(begin.m_begin),
            m_end(begin.m_begin),
            m_compare(begin.m_compare)
        {
        }

        union_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void remove_empty_ranges ()
        {
            m_end = std::remove_if(m_begin, m_end, [] (const auto & r) {return r.empty();});
        }

        void maintain_invariant ()
        {
            remove_empty_ranges();
            std::sort(m_begin, m_end, each(front) | m_compare);
        }

        void increment ()
        {
            auto current_union_end =
                std::upper_bound(m_begin, m_end, *m_begin, each(front) | m_compare);
            std::for_each(m_begin, current_union_end,
                [] (auto & range)
                {
                    range.advance_begin(1);
                });

            maintain_invariant();
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_begin->front();
        }

        bool equal (const union_iterator & that) const
        {
            assert(this->m_begin == that.m_begin);
            return std::equal(this->m_begin, this->m_end, that.m_begin, that.m_end);
        }

    private:
        outer_range_iterator m_begin;
        outer_range_iterator m_end;
        compare_type m_compare;

    };

    //!     Функция для создания итератора объединения с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно объединить, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первый элемент объединения входных диапазонов.
     */
    template <typename RandomAccessIterator, typename Compare>
    auto
        make_union_iterator
        (
            RandomAccessIterator first, RandomAccessIterator last,
            Compare compare
        )
    {
        return
            union_iterator<RandomAccessIterator, Compare>
            (
                std::move(first), std::move(last),
                compare
            );
    }

    template <typename RandomAccessRange, typename Compare>
    auto make_union_iterator (RandomAccessRange && ranges, Compare compare)
    {
        using std::begin;
        using std::end;
        return
            make_union_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges)),
                compare
            );
    }

    /*!
        \brief
            Функция для создания итератора объединения с предикатом из кортежа ссылок

        \details
            Создаёт итератор ленивого объединения нескольких диапазонов. Проход от созданного
            итератора до итератора-конца (см. перегрузку с `end_tag_t`) перечисляет все элементы
            исходных диапазонов `ranges`, которые присутствуют хотя бы в одном из этих диапазонов,
            причём в порядке их неубывания относительно операции `compare`.

        \param ranges
            Кортеж ссылок на диапазоны, которые нужно объединить.
        \param compare
            Операция, задающая отношение строгого порядка на элементах результирующего диапазона.

        \pre
            Каждый диапазон в `ranges` упорядочен относительно операции `compare`.

        \returns
            Итератор на наименьший относительно заданного отношения порядка элемент ленивого
            объединения входных диапазонов.

        \see union_iterator
     */
    template <typename ... Ranges, typename Compare>
    auto make_union_iterator (std::tuple<Ranges &...> ranges, Compare compare)
    {
        return
            make_union_iterator
            (
                burst::own_as_range(burst::apply(burst::make_range_vector, ranges)),
                std::move(compare)
            );
    }

    //!     Функция для создания итератора объединения.
    /*!
            Принимает на вход набор диапазонов, которые нужно объединить.
            Возвращает итератор на первый элемент объединения входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessIterator>
    auto make_union_iterator (RandomAccessIterator first, RandomAccessIterator last)
    {
        return union_iterator<RandomAccessIterator>(std::move(first), std::move(last));
    }

    template <typename RandomAccessRange>
    auto make_union_iterator (RandomAccessRange && ranges)
    {
        using std::begin;
        using std::end;
        return
            make_union_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges))
            );
    }

    /*!
        \brief
            Функция для создания итератора объединения из кортежа ссылок

        \returns
            Итератор ленивого объединения с отношением порядка по умолчанию.

        \see make_union_iterator
        \see union_iterator
     */
    template <typename ... Ranges>
    auto make_union_iterator (std::tuple<Ranges &...> ranges)
    {
        return
            make_union_iterator
            (
                burst::own_as_range(burst::apply(burst::make_range_vector, ranges))
            );
    }

    //!     Функция для создания итератора на конец объединения с предикатом.
    /*!
            Принимает на вход итератор на начало объединения и индикатор конца итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы
        объединения закончились.
     */
    template <typename RandomAccessIterator, typename Compare>
    auto
        make_union_iterator
        (
            iterator::end_tag_t,
            const union_iterator<RandomAccessIterator, Compare> & begin
        )
    {
        return union_iterator<RandomAccessIterator, Compare>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST_ITERATOR_UNION_ITERATOR_HPP
