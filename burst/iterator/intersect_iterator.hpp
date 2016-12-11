#ifndef BURST_ITERATOR_INTERSECT_ITERATOR_HPP
#define BURST_ITERATOR_INTERSECT_ITERATOR_HPP

#include <burst/iterator/detail/front_value_compare.hpp>
#include <burst/iterator/detail/prevent_writing.hpp>
#include <burst/iterator/detail/range_range.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/skip_to_lower_bound.hpp>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/concepts.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

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

        \tparam RandomAccessRange
            Тип принимаемого на вход внешнего диапазона. Он должен быть диапазоном произвольного
            доступа, то есть удовлетворять требованиям понятия "Random Access Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах внутренних
            диапазонов. Если пользователем явно не указана операция, то, по-умолчанию, берётся
            отношение "меньше", задаваемое функциональным объектом "std::less<>".

            Алгоритм работы.

        1. Внешний диапазон переупорядочивается так, что внутренний диапазон с наибольшим по
           заданному отношению порядка первым элементом всегда стоит последним.
        2. Поиск нового пересечения.
           а. Каждый диапазон продвигается так, чтобы его первый элемент был не меньше первого
              элемента последнего диапазона.
           б. Если диапазон закончился, то пересечений больше нет.
           в. Если последний диапазон по первому элементу стал меньше текущего, то текущий
              становится последним, и весь цикл повторяется заново.
           г. Если текущий диапазон по первому элементу равен последнему диапазону, то берётся
              следующий диапазон, и т.д.
           д. Когда все диапазоны равны по первому элементу — пересечение найдено.
        3. Каждый раз, когда необходимо найти следующий элемент в пересечении, надо "вывести из
           равновесия" набор диапазонов, продвинув каждый из них ровно на один элемент вперёд, и
           поддержать инвариант, установив диапазон с наибольшим первым элементом в конец массива,
           после чего исполнить п.2.
     */
    template
    <
        typename RandomAccessRange,
        typename Compare = std::less<>
    >
    class intersect_iterator:
        public boost::iterator_facade
        <
            intersect_iterator<RandomAccessRange, Compare>,
            detail::range_range_value_t<RandomAccessRange>,
            boost::single_pass_traversal_tag,
            detail::prevent_writing_t<detail::range_range_reference_t<RandomAccessRange>>
        >
    {
    private:
        using outer_range_type = RandomAccessRange;
        BOOST_CONCEPT_ASSERT((boost::RandomAccessRangeConcept<outer_range_type>));

        using inner_range_type = typename boost::range_value<outer_range_type>::type;
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<inner_range_type>));

        using compare_type = Compare;

        using base_type =
            boost::iterator_facade
            <
                intersect_iterator,
                detail::range_range_value_t<outer_range_type>,
                boost::single_pass_traversal_tag,
                detail::prevent_writing_t<detail::range_range_reference_t<outer_range_type>>
            >;

    public:
        explicit intersect_iterator (outer_range_type ranges, Compare compare = Compare()):
            m_ranges(),
            m_compare(compare)
        {
            if (boost::algorithm::none_of(ranges, [] (const auto & range) { return range.empty(); }))
            {
                BOOST_ASSERT((boost::algorithm::all_of(ranges,
                    [this] (const auto & range)
                    {
                        return boost::algorithm::is_sorted(range, m_compare);
                    })));

                m_ranges = std::move(ranges);
                boost::sort(m_ranges, detail::compare_by_front_value(m_compare));
                settle();
            }
        }

        intersect_iterator (iterator::end_tag_t, const intersect_iterator & begin):
            m_ranges(std::begin(begin.m_ranges), std::begin(begin.m_ranges)),
            m_compare(begin.m_compare)
        {
        }

        intersect_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            faze();
            settle();
        }

        //!     Вывести диапазоны из равновесия.
        /*!
                Приводит пересекаемые диапазоны в такое состояние, что предыдущее пересечение уже
            недостижимо, а следующее пересечение в общем случае ещё не достигнуто.
                Продвигает пересекаемые диапазоны на один элемент вперёд и устанавливает наибольший
            по первому элементу из них в конец массива диапазонов.
                Если хотя бы какой-нибудь из диапазонов в процессе продвижения закончился, то
            итератор пересечения устанавливается на конец пересечений.
         */
        void faze ()
        {
            auto max_range = m_ranges.begin();
            for (auto range = m_ranges.begin(); range != m_ranges.end(); ++range)
            {
                range->advance_begin(1);
                if (range->empty())
                {
                    scroll_to_end();
                    return;
                }
                else if (m_compare(max_range->front(), range->front()))
                {
                    max_range = range;
                }
            }
            std::swap(*max_range, m_ranges.back());
        }

        //!     Устаканить диапазоны на ближайшем пересечении.
        /*!
                Устанавливает пересекаемые диапазоны на минимальном из оставшихся элементе, который
            есть в каждом из диапазонов.
                Бежит по массиву диапазонов и продвигает каждый так, чтобы его первый элемент был
            не меньше первого элемента наибольшего диапазона (наибольший диапазон всегда ставится в
            конец массива). Если в процессе продвижения какой-либо из диапазонов стал больше
            (по первому элементу) текущего наибольшего, то он сам становится наибольшим, и процесс
            начинается заново.
                Если хотя бы какой-нибудь из диапазонов в процессе продвижения закончился, то
            итератор пересечения устанавливается на конец пересечений.
         */
        void settle ()
        {
            if (not m_ranges.empty())
            {
                auto range = m_ranges.begin();
                while (range != std::prev(m_ranges.end()))
                {
                    if (m_compare(range->front(), m_ranges.back().front()))
                    {
                        skip_to_lower_bound(*range, m_ranges.back().front(), m_compare);
                        if (range->empty())
                        {
                            scroll_to_end();
                            return;
                        }
                    }

                    if (m_compare(m_ranges.back().front(), range->front()))
                    {
                        // Возможно, тут надо продвинуть последний диапазон до нового минимума и
                        // посмотреть, нужно ли его после этого менять местами с текущим
                        // диапазоном. Возможно, это будет цикл.
                        std::swap(m_ranges.back(), *range);
                        range = m_ranges.begin();
                    }
                    else
                    {
                        ++range;
                    }
                }
            }
        }

        void scroll_to_end ()
        {
            m_ranges = outer_range_type(m_ranges.begin(), m_ranges.begin());
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
        outer_range_type m_ranges;
        compare_type m_compare;
    };

    //!     Функция для создания итератора пересечения с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первое пересечение входных диапазонов.
     */
    template <typename RandomAccessRange, typename Compare>
    auto make_intersect_iterator (RandomAccessRange ranges, Compare compare)
    {
        return intersect_iterator<RandomAccessRange, Compare>(std::move(ranges), compare);
    }

    //!     Функция для создания итератора пересечения.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь.
            Возвращает итератор на первое пересечение входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    auto make_intersect_iterator (RandomAccessRange ranges)
    {
        return intersect_iterator<RandomAccessRange>(std::move(ranges));
    }

    //!     Функция для создания итератора на конец пересечения.
    /*!
            Принимает на вход итератор на начало пересекаемых диапазонов и индикатор конца
        итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы
        пересечения закончились.
     */
    template <typename RandomAccessRange, typename Compare>
    auto
        make_intersect_iterator
        (
            iterator::end_tag_t,
            const intersect_iterator<RandomAccessRange, Compare> & begin
        )
    {
        return intersect_iterator<RandomAccessRange, Compare>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST_ITERATOR_INTERSECT_ITERATOR_HPP
