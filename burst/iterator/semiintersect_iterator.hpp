#ifndef BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP
#define BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/bind.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/count.hpp>
#include <boost/range/algorithm/count_if.hpp>
#include <boost/range/algorithm/remove_if.hpp>

#include <burst/iterator/detail/front_value_compare.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/skip_to_lower_bound.hpp>

namespace burst
{
    //!     Итератор полупересечения.
    /*!
            Полупересечение N диапазонов — это элементы, каждый из которых есть не менее, чем в M
        из этих диапазонов, 1 <= M <= N. При M = 1 полупересечение вырождается в объединение, при
        M = N — в строгое пересечение.
            Предназначен для полупересечения нескольких диапазонов одного типа "на лету", то есть
        без использования дополнительной памяти для хранения результирующего диапазона.
            Принимает на вход набор упорядоченных диапазонов и перемещается по тем элементам,
        которые есть не менее чем в M из этих диапазонов.
            Входные диапазоны рассматриваются как мультимножества.
            Полученный в результате полупересечения диапазон неизменяем. То есть из итератора можно
        только прочитать значения, но нельзя записать. Такое ограничение обусловлено тем, что
        каждый выдаваемый итератором элемент может присутствовать в нескольких из входных
        диапазонов, а значит, нельзя однозначно выбрать из них какой-то один для записи нового
        значения.
            Между элементами результирующего диапазона сохраняется заданное отношение порядка.

        \tparam ForwardRange
            Тип принимаемого на вход диапазона. Он должен быть однонаправленным, то есть
            удовлетворять требованиям понятия "Forward Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах входных диапазонов.
            Если пользователем явно не указана операция, то, по-умолчанию, берётся отношение
            "меньше", задаваемое функциональным объектом "std::less<T>".

            Алгоритм работы.

        1. Диапазоны склыдываются в массив, в котором диапазон с M-м по заданному отношению порядка
           первым элементом всегда стоит на M-м месте, все диапазоны, которые слева от M-го, меньше
           него по первому элементу, а все, которые справа, — не меньше.
        2. Поиск нового полупересечения.
           а. Каждый диапазон слева от M-го продвигается так, чтобы его первый элемент был не
              меньше первого элемента M-го диапазона.
           б. Если диапазон закончился, то он выбрасывается из рассмотрения. Когда диапазонов
              становится меньше, чем M, полупересечений больше нет.
           в. Если текущий диапазон по первому элементу стал больше M-го, то массив диапазонов
              переупорядочивается в соответствии с п.1, и весь цикл повторяется заново.
           г. Если текущий диапазон по первому элементу равен M-му диапазону, то берётся следующий
              диапазон, и т.д.
           д. Когда первые M диапазонов равны по первому элементу — полупересечение найдено.
        3. Каждый раз, когда необходимо найти следующий элемент в полупересечении, надо "вывести из
           равновесия" набор диапазонов, продвинув ровно на один элемент вперёд первые M из них, а
           также все диапазоны справа от M-го, равные ему по первому элементу, и поддержать
           инвариант п.1, после чего исполнить п.2.
     */
    template
    <
        typename ForwardRange,
        typename Compare = std::less<typename ForwardRange::value_type>
    >
    class semiintersect_iterator:
        public boost::iterator_facade
        <
            semiintersect_iterator<ForwardRange, Compare>,
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

        using range_container_type = std::vector<range_type>;
        using range_iterator = typename range_container_type::iterator;
        using range_const_iterator = typename range_container_type::const_iterator;

        using base_type =
            boost::iterator_facade
            <
                semiintersect_iterator<range_type, compare_type>,
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
        semiintersect_iterator (const ForwardRange1 & ranges, std::size_t min_items, Compare compare = Compare()):
            m_ranges(),
            m_min_items(min_items),
            m_compare(compare)
        {
            BOOST_STATIC_ASSERT(boost::is_same<typename ForwardRange1::value_type, range_type>::value);
            BOOST_ASSERT(boost::algorithm::all_of(ranges, boost::bind(&boost::algorithm::is_sorted<range_type, compare_type>, _1, m_compare)));
            BOOST_ASSERT_MSG(min_items > 0, "Невозможно получить полупересечение из нуля элементов.");

            auto non_empty = [] (const auto & range) { return not range.empty(); };

            auto non_empty_range_count = static_cast<std::size_t>(boost::count_if(ranges, non_empty));
            if (non_empty_range_count >= m_min_items)
            {
                m_ranges.reserve(non_empty_range_count);
                std::copy_if(ranges.begin(), ranges.end(), std::back_inserter(m_ranges), non_empty);

                maintain_invariant();
                settle();
            }
        }

        semiintersect_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        //!     Поддержать инвариант, необходимый для поиска полупересечений.
        /*!
                Подробно инвариант описан в п.1 алгоритма работы.
         */
        void maintain_invariant ()
        {
            auto candidate = semiintersection_candidate();
            std::nth_element(m_ranges.begin(), candidate, m_ranges.end(), detail::compare_by_front_value(m_compare));
        }

        void increment ()
        {
            faze();

            if (not is_end())
            {
                settle();
            }
        }

        //!     Вывести диапазоны из равновесия.
        /*!
                Приводит пересекаемые диапазоны в такое состояние, что предыдущее полупересечение
            уже недостижимо, а следующее полупересечение в общем случае ещё не достигнуто.
                Продвигает все диапазоны, установленные на текущем полупересечении (то есть все
            первые M+ диапазонов, у которых равны первые элементы), на один элемент вперёд и
            восстанавливает инвариант.
                Если в процессе продвижения закончилось столько диапазонов, что их стало меньше,
            чем m_min_items, то итератор пересечения устанавливается на конец пересечений.
         */
        void faze ()
        {
            std::for_each(m_ranges.begin(), semiintersection_end(),
                [] (auto & range)
                {
                    range.advance_begin(1);
                });

            auto is_empty = [] (const auto & range) { return range.empty(); };

            auto non_empty_end = boost::remove_if(m_ranges, is_empty);
            if (static_cast<std::size_t>(std::distance(m_ranges.begin(), non_empty_end)) >= m_min_items)
            {
                m_ranges.erase(non_empty_end, m_ranges.end());
                maintain_invariant();
            }
            else
            {
                scroll_to_end();
            }
        }

        //!     Получить конец полупересечения.
        /*!
                Устанавливает диапазоны справа от кандидата так, что все диапазоны, равные ему по
            первому элементу, стоят сразу за ним.
                Возвращает итератор на первый диапазон, первый элемент которого больше первого
            элемента кандидата.
         */
        range_iterator semiintersection_end ()
        {
            auto candidate = semiintersection_candidate();

            auto front_values = boost::make_iterator_range(std::next(candidate), m_ranges.end()) |
                boost::adaptors::transformed([] (const auto & range)
                {
                    return range.front();
                });

            auto last_equal_to_candidate = candidate + boost::count(front_values, candidate->front());
            std::nth_element(candidate, last_equal_to_candidate, m_ranges.end(), detail::compare_by_front_value(m_compare));

            return std::next(last_equal_to_candidate);
        }

        //!     Устаканить диапазоны на ближайшем полупересечении.
        /*!
                Устанавливает диапазоны на минимальном из оставшихся элементе, который есть не
            менее, чем в M из рассматриваемых диапазонов.
                Бежит по массиву диапазонов от первого до кандидата и продвигает каждый из них так,
            чтобы его первый элемент был не меньше первого элемента кандидата. Если в процессе
            продвижения какой-либо из диапазонов закончился или стал больше (по первому элементу)
            кандидата, то диапазоны переупорядочиваются, выбирается новый кандидат, и цикл
            начинается сначала.
                Если слишком много диапазонов закончилось, то есть их стало меньше M, то итератор
            полупересечения устанавливается на конец полупересечений.
         */
        void settle ()
        {
            while (not is_end())
            {
                auto skipped_until = skip_while_less(m_ranges.begin(), semiintersection_candidate());
                if (skipped_until == semiintersection_candidate())
                {
                    // Полупересечение найдено.
                    break;
                }
                else
                {
                    if (skipped_until->empty())
                    {
                        remove_empty_range(skipped_until);
                    }
                    else
                    {
                        maintain_invariant();
                    }

                }
            }
        }

        //!     Продвинуть диапазоны до кандидата.
        /*!
                Пытается продвинуть диапазоны [range, candidate) так, чтобы их первый элемент был
            не меньше первого элемента диапазона candidate.
                Если после продвижения некоторый диапазон опустевает, или его первый элемент
            становится больше первого элемента кандидата, то процесс останавливается и возвращается
            итератор на этот диапазон.
         */
        range_iterator skip_while_less (range_iterator range, range_iterator candidate)
        {
            while (range != candidate)
            {
                if (m_compare(range->front(), candidate->front()))
                {
                    skip_to_lower_bound(*range, candidate->front(), m_compare);
                    if (range->empty() || m_compare(candidate->front(), range->front()))
                    {
                        break;
                    }
                }

                ++range;
            }

            return range;
        }

        //!     Устранить из рассмотрения опустевший диапазон.
        /*!
                Если после удаления этого диапазона их по-прежнему будет достаточно для
            полупересечения, то нужно удалить диапазон и поддержать инвариант.
                Если же диапазонов станет меньше необходимого минимума, то итератор полупересечений
            надо сразу установить на конец полупересечений.
         */
        void remove_empty_range (range_iterator empty_range)
        {
            if (m_ranges.size() - 1 >= m_min_items)
            {
                std::rotate(empty_range, std::next(empty_range), m_ranges.end());
                m_ranges.pop_back();

                maintain_invariant();
            }
            else
            {
                scroll_to_end();
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

        bool equal (const semiintersect_iterator & that) const
        {
            return this->m_ranges == that.m_ranges;
        }

        range_iterator semiintersection_candidate ()
        {
            return m_ranges.begin() + std::distance(m_ranges.cbegin(), semiintersection_candidate_impl());
        }

        bool is_end () const
        {
            return m_ranges.empty();
        }

        //!     Итератор на кандидата полупересечения.
        /*!
                Кандидат полупересечения — это M-й по первому элементу диапазон.
         */
        range_const_iterator semiintersection_candidate_impl () const
        {
            using difference_type =
                typename std::iterator_traits
                <
                    range_const_iterator
                >
                ::difference_type;

            return m_ranges.cbegin() + static_cast<difference_type>(m_min_items - 1);
        }

    private:
        range_container_type m_ranges;
        std::size_t m_min_items;
        compare_type m_compare;
    };

    //!     Функция для создания итератора полупересечения с предикатом.
    /*!
            Принимает на вход набор диапазонов, для которых нужно найти полупересечение,
        минимальное количество элементов в полупересечении и операцию, задающую отношение строгого
        порядка на элементах этих диапазонов.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первое полупересечение входных диапазонов.
     */
    template <typename ForwardRange, typename Compare>
    semiintersect_iterator
    <
        typename ForwardRange::value_type,
        Compare
    >
    make_semiintersect_iterator (const ForwardRange & ranges, std::size_t min_items, Compare compare)
    {
        return semiintersect_iterator<typename ForwardRange::value_type, Compare>(ranges, min_items, compare);
    }

    //!     Функция для создания итератора полупересечения.
    /*!
            Принимает на вход набор диапазонов, для которых нужно найти полупересечение, и
        минимальное количество элементов в полупересечении.
            Возвращает итератор на первое полупересечение входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename ForwardRange>
    semiintersect_iterator
    <
        typename ForwardRange::value_type
    >
    make_semiintersect_iterator (const ForwardRange & ranges, std::size_t min_items)
    {
        return semiintersect_iterator<typename ForwardRange::value_type>(ranges, min_items);
    }

    //!     Функция для создания итератора на конец полупересечения с предикатом.
    /*!
            Принимает на вход набор диапазонов, предикат и индикатор конца итератора.
            Набор диапазонов и предикат не используются, они нужны только для автоматического
        вывода типа итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что полупересечения
        закончились.
     */
    template <typename ForwardRange, typename Compare>
    semiintersect_iterator
    <
        typename ForwardRange::value_type,
        Compare
    >
    make_semiintersect_iterator (const ForwardRange &, Compare, iterator::end_tag_t)
    {
        return semiintersect_iterator<typename ForwardRange::value_type, Compare>();
    }

    //!     Функция для создания итератора на конец полупересечения.
    /*!
            Принимает на вход набор диапазонов, который не используется, а нужен только для
        автоматического вывода типа итератора.
            Возвращает итератор на конец последовательности полупересечений.
            Отношение порядка берётся по-умолчанию.
     */
    template <typename ForwardRange>
    semiintersect_iterator
    <
        typename ForwardRange::value_type
    >
    make_semiintersect_iterator (const ForwardRange &, iterator::end_tag_t)
    {
        return semiintersect_iterator<typename ForwardRange::value_type>();
    }
} // namespace burst

#endif // BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP
