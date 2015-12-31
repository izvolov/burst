#ifndef BURST_ITERATOR_INTERSECT_ITERATOR_HPP
#define BURST_ITERATOR_INTERSECT_ITERATOR_HPP

#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/concepts.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <burst/iterator/detail/front_value_compare.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/skip_to_lower_bound.hpp>

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

        \tparam ForwardRange
            Тип принимаемого на вход диапазона. Он должен быть однонаправленным, то есть
            удовлетворять требованиям понятия "Forward Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах входных диапазонов.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<T>".

            Алгоритм работы.

        1. Диапазоны склыдываются в массив, в котором диапазон с наибольшим по заданному отношению
           порядка первым элементом всегда стоит последним.
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
        typename ForwardRange,
        typename Compare = std::less<typename ForwardRange::value_type>
    >
    class intersect_iterator:
        public boost::iterator_facade
        <
            intersect_iterator<ForwardRange, Compare>,
            typename ForwardRange::value_type,
            boost::forward_traversal_tag,
            typename std::conditional
            <
                std::is_lvalue_reference<typename ForwardRange::reference>::value,
                typename std::remove_reference<typename ForwardRange::reference>::type const &,
                typename ForwardRange::reference
            >
            ::type
        >
    {
    private:
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<ForwardRange>));
        using range_type = ForwardRange;
        using compare_type = Compare;

        using base_type =
            boost::iterator_facade
            <
                intersect_iterator,
                typename range_type::value_type,
                boost::forward_traversal_tag,
                typename std::conditional
                <
                    std::is_lvalue_reference<typename range_type::reference>::value,
                    typename std::remove_reference<typename range_type::reference>::type const &,
                    typename range_type::reference
                >
                ::type
            >;

    public:
        template <typename ForwardRange1>
        explicit intersect_iterator (const ForwardRange1 & ranges, Compare compare = Compare()):
            m_ranges(),
            m_compare(compare)
        {
            BOOST_STATIC_ASSERT(boost::is_same<typename ForwardRange1::value_type, range_type>::value);

            if (boost::algorithm::none_of(ranges, boost::bind(&range_type::empty, _1)))
            {
                BOOST_ASSERT(boost::algorithm::all_of(ranges.begin(), ranges.end(), boost::bind(&boost::algorithm::is_sorted<range_type, Compare>, _1, compare)));
                m_ranges.assign(ranges.begin(), ranges.end());
                boost::sort(m_ranges, detail::compare_by_front_value(m_compare));
                settle();
            }
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
            m_ranges.clear();
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
        std::vector<range_type> m_ranges;
        compare_type m_compare;
    };

    //!     Функция для создания итератора пересечения с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первое пересечение входных диапазонов.
     */
    template <typename ForwardRange, typename Compare>
    intersect_iterator
    <
        typename ForwardRange::value_type,
        Compare
    >
    make_intersect_iterator (const ForwardRange & ranges, Compare compare)
    {
        return intersect_iterator<typename ForwardRange::value_type, Compare>(ranges, compare);
    }

    //!     Функция для создания итератора пересечения.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь.
            Возвращает итератор на первое пересечение входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename ForwardRange>
    intersect_iterator
    <
        typename ForwardRange::value_type
    >
    make_intersect_iterator (const ForwardRange & ranges)
    {
        return intersect_iterator<typename ForwardRange::value_type>(ranges);
    }

    //!     Функция для создания итератора на конец пересечения с предикатом.
    /*!
            Принимает на вход набор диапазонов, предикат и индикатор конца итератора.
            Набор диапазонов и предикат не используются, они нужны только для автоматического
        вывода типа итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что пересечения
        закончились.
     */
    template <typename ForwardRange, typename Compare>
    intersect_iterator
    <
        typename ForwardRange::value_type,
        Compare
    >
    make_intersect_iterator (const ForwardRange &, Compare, iterator::end_tag_t)
    {
        return intersect_iterator<typename ForwardRange::value_type, Compare>();
    }

    //!     Функция для создания итератора на конец пересечения.
    /*!
            Принимает на вход набор диапазонов, который не используется, а нужен только для
        автоматического вывода типа итератора.
            Возвращает итератор на конец последовательности пересечений.
            Отношение порядка берётся по-умолчанию.
     */
    template <typename ForwardRange>
    intersect_iterator
    <
        typename ForwardRange::value_type
    >
    make_intersect_iterator (const ForwardRange &, iterator::end_tag_t)
    {
        return intersect_iterator<typename ForwardRange::value_type>();
    }
} // namespace burst

#endif // BURST_ITERATOR_INTERSECT_ITERATOR_HPP
