#ifndef BURST_ITERATOR_MERGE_ITERATOR_HPP
#define BURST_ITERATOR_MERGE_ITERATOR_HPP

#include <burst/container/access/front.hpp>
#include <burst/functional/each.hpp>
#include <burst/functional/invert.hpp>
#include <burst/iterator/detail/uniform_range_tuple_please.hpp>
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
#include <tuple>
#include <utility>

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
            Алгоритм деструктивен по отношению к внешнему и всем внутренним диапазонам. Поэтому
        итератор слияния однопроходный.

        \tparam RandomAccessIterator
            Тип принимаемого на вход итератора внешнего диапазона. Он должен быть итератором
            произвольного доступа, то есть удовлетворять требованиям понятия "Random Access Iterator".
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
        typename RandomAccessIterator,
        typename Compare = std::less<>
    >
    class merge_iterator:
        public boost::iterator_facade
        <
            merge_iterator<RandomAccessIterator, Compare>,
            range_value_t<iterator_value_t<RandomAccessIterator>>,
            boost::single_pass_traversal_tag,
            range_reference_t<iterator_value_t<RandomAccessIterator>>
        >
    {
    private:
        using outer_range_iterator = RandomAccessIterator;
        BOOST_CONCEPT_ASSERT((boost::RandomAccessIteratorConcept<outer_range_iterator>));

        using inner_range_type = iterator_value_t<outer_range_iterator>;
        BOOST_CONCEPT_ASSERT((boost::SinglePassRangeConcept<inner_range_type>));

        using base_type =
            boost::iterator_facade
            <
                merge_iterator,
                range_value_t<inner_range_type>,
                boost::single_pass_traversal_tag,
                range_reference_t<inner_range_type>
            >;

    public:
        explicit merge_iterator (outer_range_iterator first, outer_range_iterator last, Compare compare = Compare()):
            m_begin(std::move(first)),
            m_end(std::move(last)),
            m_compare(compare)
        {
            // Чтобы провести эту проверку, внутренние диапазоны должны быть однонаправленными.
            // Подумать, как сделать эту проверку не требуя однонаправленности.
            // BOOST_ASSERT(std::all_of(m_begin, m_end,
            //     [& compare] (const auto & range)
            //     {
            //         return boost::algorithm::is_sorted(range, compare);
            //     }));

            remove_empty_ranges();
            std::make_heap(m_begin, m_end, each(front) | invert(m_compare));
        }

        merge_iterator (iterator::end_tag_t, const merge_iterator & begin):
            m_begin(begin.m_begin),
            m_end(begin.m_begin),
            m_compare(begin.m_compare)
        {
        }

        merge_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void remove_empty_ranges ()
        {
            m_end = std::remove_if(m_begin, m_end, [] (const auto & r) {return r.empty();});
        }

        void increment ()
        {
            std::pop_heap(m_begin, m_end, each(front) | invert(m_compare));
            auto & range = *std::prev(m_end);

            range.advance_begin(1);
            if (not range.empty())
            {
                std::push_heap(m_begin, m_end, each(front) | invert(m_compare));
            }
            else
            {
                --m_end;
            }
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_begin->front();
        }

        bool equal (const merge_iterator & that) const
        {
            assert(this->m_begin == that.m_begin);
            return std::equal(this->m_begin, this->m_end, that.m_begin, that.m_end);
        }

    private:
        outer_range_iterator m_begin;
        outer_range_iterator m_end;

        Compare m_compare;
    };

    //!     Функция для создания итератора слияния с предикатом.
    /*!
            Принимает на вход диапазон диапазонов, которые нужно слить, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на наименьший относительно заданного отношения порядка элемент
        среди входных диапазонов.
     */
    template <typename RandomAccessIterator, typename Compare>
    auto make_merge_iterator (RandomAccessIterator first, RandomAccessIterator last, Compare compare)
    {
        return merge_iterator<RandomAccessIterator, Compare>(std::move(first), std::move(last), compare);
    }

    template <typename RandomAccessRange, typename Compare>
    auto make_merge_iterator (RandomAccessRange && ranges, Compare compare)
    {
        using std::begin;
        using std::end;
        return
            make_merge_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges)),
                compare
            );
    }

    /*!
        \brief
            Функция для создания итератора слияния с предикатом из кортежа ссылок

        \details
            Создаёт итератор ленивого слияния нескольких диапазонов. Проход от созданного итератора
            до итератора-конца (см. перегрузку с `end_tag_t`) перечисляет все элементы исходных
            диапазонов `ranges` в порядке их неубывания относительно операции `compare`.

        \param ranges
            Кортеж ссылок на диапазоны, которые нужно слить.
        \param compare
            Операция, задающая отношение строгого порядка на элементах результирующего диапазона.

        \pre
            Каждый диапазон в `ranges` упорядочен относительно операции `compare`.

        \returns
            Итератор на наименьший относительно заданного отношения порядка элемент ленивого
            слияния входных диапазонов.

        \see merge_iterator
     */
    template <typename ... Ranges, typename Compare>
    auto make_merge_iterator (std::tuple<Ranges &...> ranges, Compare compare)
    {
        auto common_ranges = detail::uniform_range_tuple_please(ranges);
        return
            make_merge_iterator
            (
                burst::own_as_range(burst::apply(burst::make_range_vector, common_ranges)),
                std::move(compare)
            );
    }

    //!     Функция для создания итератора слияния.
    /*!
            Принимает на вход диапазон диапазонов, которые нужно слить.
            Возвращает итератор на наименьший элемент среди входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessIterator>
    auto make_merge_iterator (RandomAccessIterator first, RandomAccessIterator last)
    {
        return merge_iterator<RandomAccessIterator>(std::move(first), std::move(last));
    }

    template <typename RandomAccessRange>
    auto make_merge_iterator (RandomAccessRange && ranges)
    {
        using std::begin;
        using std::end;
        return
            make_merge_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges))
            );
    }

    /*!
        \brief
            Функция для создания итератора слияния из кортежа ссылок

        \returns
            Итератор ленивого слияния с отношением порядка по умолчанию.

        \see make_merge_iterator
        \see merge_iterator
     */
    template <typename ... Ranges>
    auto make_merge_iterator (std::tuple<Ranges &...> ranges)
    {
        auto common_ranges = detail::uniform_range_tuple_please(ranges);
        return make_merge_iterator(own_as_range(burst::apply(make_range_vector, common_ranges)));
    }

    //!     Функция для создания итератора на конец слияния с предикатом.
    /*!
            Принимает на вход итератор на начало сливаемых диапазонов и индикатор конца итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы слияния
        закончились.
     */
    template <typename RandomAccessIterator, typename Compare>
    auto make_merge_iterator (iterator::end_tag_t, const merge_iterator<RandomAccessIterator, Compare> & begin)
    {
        return merge_iterator<RandomAccessIterator, Compare>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST_ITERATOR_MERGE_ITERATOR_HPP
