#ifndef BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP
#define BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP

#include <burst/container/access/front.hpp>
#include <burst/functional/each.hpp>
#include <burst/iterator/detail/prevent_writing.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/skip_to_lower_bound.hpp>
#include <burst/type_traits/iterator_difference.hpp>
#include <burst/type_traits/iterator_value.hpp>
#include <burst/type_traits/range_reference.hpp>
#include <burst/type_traits/range_value.hpp>

#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/count.hpp>
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
            Полупересечение N диапазонов с параметром M — это элементы, каждый из которых есть не
        менее, чем в M из этих диапазонов, 1 <= M <= N. При M = 1 полупересечение вырождается в
        объединение, при M = N — в строгое пересечение.
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

        \tparam RandomAccessIterator
            Тип итератора принимаемого на вход внешнего диапазона. Он должен быть диапазоном
            произвольного доступа, то есть удовлетворять требованиям понятия "Random Access
            Iterator".
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
        typename RandomAccessIterator,
        typename Compare = std::less<>
    >
    class semiintersect_iterator:
        public boost::iterator_facade
        <
            semiintersect_iterator<RandomAccessIterator, Compare>,
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
                semiintersect_iterator,
                range_value_t<inner_range_type>,
                boost::single_pass_traversal_tag,
                detail::prevent_writing_t<range_reference_t<inner_range_type>>
            >;

    public:
        semiintersect_iterator
            (
                outer_range_iterator first, outer_range_iterator last,
                std::size_t min_items,
                Compare compare = Compare()
            ):
            m_begin(std::move(first)),
            m_end(std::move(last)),
            m_min_items(min_items),
            m_compare(compare)
        {
            BOOST_ASSERT(std::all_of(m_begin, m_end,
                [this] (const auto & range)
                {
                    return boost::algorithm::is_sorted(range, m_compare);
                }));
            BOOST_ASSERT_MSG(min_items > 0, "Невозможно получить полупересечение из нуля элементов.");

            settle();
        }

        semiintersect_iterator (iterator::end_tag_t, const semiintersect_iterator & begin):
            m_begin(begin.m_begin),
            m_end(begin.m_begin),
            m_min_items(begin.m_min_items),
            m_compare(begin.m_compare)
        {
        }

        semiintersect_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void remove_empty_ranges ()
        {
            m_end = std::remove_if(m_begin, m_end, [] (const auto & r) {return r.empty();});
        }

        //!     Поддержать инвариант, необходимый для поиска полупересечений.
        /*!
                Подробно инвариант описан в п.1 алгоритма работы.
         */
        void maintain_invariant ()
        {
            std::nth_element(m_begin, semiintersection_candidate(), m_end, each(front) | m_compare);
        }

        //!     Продвижение к следующему полупересечению.
        /*!
                Продвигает все диапазоны, установленные на текущем полупересечении (то есть все
            первые M+ диапазонов, у которых равны первые элементы), на один элемент вперёд. В
            результате этого пересекаемые диапазоны приходят в такое состояние, что предыдущее
            полупересечение уже недостижимо, а следующее полупересечение в общем случае ещё не
            достигнуто.
                Затем запускает процесс поиска нового полупересечения.
         */
        void increment ()
        {
            std::for_each(m_begin, semiintersection_end(),
                [] (auto & range)
                {
                    range.advance_begin(1);
                });

            settle();
        }

        //!     Устаканить диапазоны на ближайшем полупересечении.
        /*!
                Выбрасывает из рассмотрения пустые диапазоны, и если их осталось не меньше M, то
            восстанавливает инвариант и запускает функцию поиска нового полупересечения.
                Если в процессе продвижения закончилось столько диапазонов, что их стало меньше,
            чем M, то итератор устанавливается на конец полупересечений.
         */
        void settle ()
        {
            remove_empty_ranges();
            if (range_count() >= m_min_items)
            {
                maintain_invariant();
                next_semiintersection();
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
                boost::make_iterator_range(std::next(candidate), m_end)
                    | boost::adaptors::transformed([] (const auto & range)
                        {
                            return range.front();
                        });

            auto last_equal_to_candidate = candidate + boost::count(front_values, candidate->front());
            std::nth_element(candidate, last_equal_to_candidate, m_end, each(front) | m_compare);

            return std::next(last_equal_to_candidate);
        }

        //!     Поиск нового полупересечения.
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
        void next_semiintersection ()
        {
            while (not is_end())
            {
                auto skipped_until = skip_while_less(m_begin, semiintersection_candidate());
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
            if (range_count() > m_min_items)
            {
                std::rotate(empty_range, std::next(empty_range), m_end);
                --m_end;

                maintain_invariant();
            }
            else
            {
                scroll_to_end();
            }
        }

        void scroll_to_end ()
        {
            m_end = m_begin;
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_begin->front();
        }

        bool equal (const semiintersect_iterator & that) const
        {
            assert(this->m_begin == that.m_begin);
            return std::equal(this->m_begin, this->m_end, that.m_begin, that.m_end);
        }

        //!     Итератор на кандидата полупересечения.
        /*!
                Кандидат полупересечения — это M-й по первому элементу диапазон.
         */
        outer_range_iterator semiintersection_candidate ()
        {
            using difference_type = iterator_difference_t<outer_range_iterator>;
            return m_begin + static_cast<difference_type>(m_min_items - 1);
        }

        bool is_end () const
        {
            return m_begin == m_end;
        }

        std::size_t range_count () const
        {
            return static_cast<std::size_t>(std::distance(m_begin, m_end));
        }

    private:
        outer_range_iterator m_begin;
        outer_range_iterator m_end;
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
    template <typename RandomAccessIterator, typename Integral, typename Compare>
    auto
        make_semiintersect_iterator
        (
            RandomAccessIterator first, RandomAccessIterator last,
            Integral min_items,
            Compare compare
        )
    {
        assert(min_items > 0);
        return
            semiintersect_iterator<RandomAccessIterator, Compare>
            (
                std::move(first), std::move(last),
                static_cast<std::size_t>(min_items),
                compare
            );
    }

    template <typename RandomAccessRange, typename Integral, typename Compare>
    auto
        make_semiintersect_iterator
        (
            RandomAccessRange && ranges,
            Integral min_items,
            Compare compare
        )
    {
        using std::begin;
        using std::end;
        return
            make_semiintersect_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges)),
                min_items,
                compare
            );
    }

    //!     Функция для создания итератора полупересечения.
    /*!
            Принимает на вход набор диапазонов, для которых нужно найти полупересечение, и
        минимальное количество элементов в полупересечении.
            Возвращает итератор на первое полупересечение входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessIterator, typename Integral>
    auto
        make_semiintersect_iterator
        (
            RandomAccessIterator first, RandomAccessIterator last,
            Integral min_items
        )
    {
        assert(min_items > 0);
        return
            semiintersect_iterator<RandomAccessIterator>
            (
                std::move(first), std::move(last),
                static_cast<std::size_t>(min_items)
            );
    }

    template <typename RandomAccessRange, typename Integral>
    auto make_semiintersect_iterator (RandomAccessRange && ranges, Integral min_items)
    {
        using std::begin;
        using std::end;
        return
            make_semiintersect_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges)),
                min_items
            );
    }

    //!     Функция для создания итератора на конец полупересечения.
    /*!
            Принимает на вход итератор на начало полупересечения и индикатор конца итератора.
            Возвращает итератор-конец, который, если до него дойти, покажет, что элементы
        полупересечения закончились.
     */
    template <typename RandomAccessIterator, typename Compare>
    auto
        make_semiintersect_iterator
        (
            iterator::end_tag_t,
            const semiintersect_iterator<RandomAccessIterator, Compare> & begin
        )
    {
        return semiintersect_iterator<RandomAccessIterator, Compare>(iterator::end_tag, begin);
    }
} // namespace burst

#endif // BURST_ITERATOR_SEMIINTERSECT_ITERATOR_HPP
