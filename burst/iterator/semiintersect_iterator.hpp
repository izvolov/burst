#ifndef BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP
#define BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP

#include <burst/iterator/detail/front_value_compare.hpp>
#include <burst/iterator/detail/range_range.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/skip_to_lower_bound.hpp>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/count.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/range/concepts.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

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

        \tparam RandomAccessRange
            Тип принимаемого на вход внешнего диапазона. Он должен быть диапазоно произвольного
            доступа, то есть удовлетворять требованиям понятия "Random Access Range".
        \tparam Compare
            Бинарная операция, задающая отношение строгого порядка на элементах внутренних
            диапазонов. Если пользователем явно не указана операция, то, по-умолчанию, берётся
            отношение "меньше", задаваемое функциональным объектом "std::less<>".

            Алгоритм работы.

        1. Внешний диапазон переупорядочивается так, что диапазон с M-м по заданному отношению
           порядка первым элементом всегда стоит на M-м месте, все диапазоны, которые слева от
           M-го, меньше него по первому элементу, а все, которые справа, — не меньше.
        2. Поиск нового полупересечения.
           а. Каждый диапазон слева от M-го продвигается так, чтобы его первый элемент был не
              меньше первого элемента M-го диапазона.
           б. Если диапазон закончился, то он выбрасывается из рассмотрения. Когда диапазонов
              становится меньше, чем M, полупересечений больше нет.
           в. Если текущий диапазон по первому элементу стал больше M-го, то внешний диапазон
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
        typename RandomAccessRange,
        typename Compare = std::less<>
    >
    class semiintersect_iterator:
        public boost::iterator_facade
        <
            semiintersect_iterator<RandomAccessRange, Compare>,
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
    private:
        using outer_range_type = RandomAccessRange;
        BOOST_CONCEPT_ASSERT((boost::RandomAccessRangeConcept<outer_range_type>));

        using inner_range_type = typename boost::range_value<outer_range_type>::type;
        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<inner_range_type>));

        using compare_type = Compare;

        using outer_range_iterator = typename boost::range_iterator<outer_range_type>::type;

        using base_type =
            boost::iterator_facade
            <
                semiintersect_iterator,
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
        semiintersect_iterator (outer_range_type ranges, std::size_t min_items, Compare compare = Compare()):
            m_ranges(std::move(ranges)),
            m_min_items(min_items),
            m_compare(compare)
        {
            BOOST_ASSERT(boost::algorithm::all_of(m_ranges,
                [this] (const auto & range)
                {
                    return boost::algorithm::is_sorted(range, m_compare);
                }));
            BOOST_ASSERT_MSG(min_items > 0, "Невозможно получить полупересечение из нуля элементов.");

            remove_empty_ranges();
            if (m_ranges.size() >= m_min_items)
            {
                maintain_invariant();
                settle();
            }
            else
            {
                scroll_to_end();
            }
        }

        semiintersect_iterator (const semiintersect_iterator & begin, iterator::end_tag_t):
            m_ranges(std::begin(begin.m_ranges), std::begin(begin.m_ranges)),
            m_min_items(begin.m_min_items),
            m_compare(begin.m_compare)
        {
        }

        semiintersect_iterator () = default;

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

        //!     Поддержать инвариант, необходимый для поиска полупересечений.
        /*!
                Подробно инвариант описан в п.1 алгоритма работы.
         */
        void maintain_invariant ()
        {
            std::nth_element
            (
                m_ranges.begin(), semiintersection_candidate(), m_ranges.end(),
                detail::compare_by_front_value(m_compare)
            );
        }

        void increment ()
        {
            faze();
            settle();
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

            remove_empty_ranges();
            if (m_ranges.size() >= m_min_items)
            {
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
        outer_range_iterator semiintersection_end ()
        {
            auto candidate = semiintersection_candidate();

            auto front_values =
                boost::make_iterator_range(std::next(candidate), m_ranges.end())
                    | boost::adaptors::transformed([] (const auto & range)
                        {
                            return range.front();
                        });

            auto last_equal_to_candidate = candidate + boost::count(front_values, candidate->front());
            std::nth_element
            (
                candidate, last_equal_to_candidate, m_ranges.end(),
                detail::compare_by_front_value(m_compare)
            );

            return std::next(last_equal_to_candidate);
        }

        //!     Устаканить диапазоны на ближайшем полупересечении.
        /*!
                Устанавливает диапазоны на минимальном из оставшихся элементе, который есть не
            менее, чем в M из рассматриваемых диапазонов.
                Бежит по внешнему диапазону от первого до кандидата и продвигает каждый из них так,
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
                    BOOST_ASSERT(not m_compare(skipped_until->front(), semiintersection_candidate()->front()));
                    BOOST_ASSERT(not m_compare(semiintersection_candidate()->front(), skipped_until->front()));
                    break; // Полупересечение найдено.
                }
                else if (skipped_until->empty())
                {
                    drop_empty_range(skipped_until);
                }
                else // Текущий диапазон больше кандидата.
                {
                    BOOST_ASSERT(m_compare(semiintersection_candidate()->front(), skipped_until->front()));
                    maintain_invariant();
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
        outer_range_iterator skip_while_less (outer_range_iterator range, outer_range_iterator candidate)
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

        //!     Выбросить из рассмотрения опустевший диапазон.
        /*!
                Если после удаления этого диапазона их по-прежнему будет достаточно для
            полупересечения, то нужно удалить диапазон и поддержать инвариант.
                Если же диапазонов станет меньше необходимого минимума, то итератор полупересечений
            надо сразу установить на конец полупересечений.
         */
        void drop_empty_range (outer_range_iterator empty_range)
        {
            if (m_ranges.size() - 1 >= m_min_items)
            {
                std::rotate(empty_range, std::next(empty_range), m_ranges.end());
                m_ranges.advance_end(-1);

                maintain_invariant();
            }
            else
            {
                scroll_to_end();
            }
        }

        void scroll_to_end ()
        {
            m_ranges = outer_range_type(std::begin(m_ranges), std::begin(m_ranges));
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

        //!     Итератор на кандидата полупересечения.
        /*!
                Кандидат полупересечения — это M-й по первому элементу диапазон.
         */
        outer_range_iterator semiintersection_candidate ()
        {
            using difference_type =
                typename std::iterator_traits
                <
                    outer_range_iterator
                >
                ::difference_type;

            return m_ranges.begin() + static_cast<difference_type>(m_min_items - 1);
        }

        bool is_end () const
        {
            return m_ranges.empty();
        }

    private:
        outer_range_type m_ranges;
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
    template <typename RandomAccessRange, typename Compare>
    auto make_semiintersect_iterator (RandomAccessRange ranges, std::size_t min_items, Compare compare)
    {
        return semiintersect_iterator<RandomAccessRange, Compare>(std::move(ranges), min_items, compare);
    }

    //!     Функция для создания итератора полупересечения.
    /*!
            Принимает на вход набор диапазонов, для которых нужно найти полупересечение, и
        минимальное количество элементов в полупересечении.
            Возвращает итератор на первое полупересечение входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessRange>
    auto make_semiintersect_iterator (RandomAccessRange ranges, std::size_t min_items)
    {
        return semiintersect_iterator<RandomAccessRange>(std::move(ranges), min_items);
    }

    //!     Функция для создания итератора на конец полупересечения.
    /*!
            Принимает на вход итератор на начало полупересечения и индикатор конца итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы
        полупересечения закончились.
     */
    template <typename RandomAccessRange, typename Compare>
    auto
        make_semiintersect_iterator
        (
            const semiintersect_iterator<RandomAccessRange, Compare> & begin,
            iterator::end_tag_t
        )
    {
        return semiintersect_iterator<RandomAccessRange, Compare>(begin, iterator::end_tag);
    }
} // namespace burst

#endif // BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP
