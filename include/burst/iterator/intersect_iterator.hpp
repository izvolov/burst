#ifndef BURST_ITERATOR_INTERSECT_ITERATOR_HPP
#define BURST_ITERATOR_INTERSECT_ITERATOR_HPP

#include <burst/container/access/front.hpp>
#include <burst/functional/each.hpp>
#include <burst/iterator/detail/prevent_writing.hpp>
#include <burst/iterator/end_tag.hpp>
#include <burst/range/make_range_vector.hpp>
#include <burst/range/own_as_range.hpp>
#include <burst/range/skip_to_lower_bound.hpp>
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
            Алгоритм деструктивен по отношению к внешнему и всем внутренним диапазонам. Поэтому
        итератор пересечения однопроходный.

        \tparam RandomAccessIterator
            Тип итератора принимаемого на вход внешнего диапазона. Он должен быть диапазоном
            произвольного доступа, то есть удовлетворять требованиям понятия "Random Access
            Iterator".
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
        typename RandomAccessIterator,
        typename Compare = std::less<>
    >
    class intersect_iterator:
        public boost::iterator_facade
        <
            intersect_iterator<RandomAccessIterator, Compare>,
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
                intersect_iterator,
                range_value_t<inner_range_type>,
                boost::single_pass_traversal_tag,
                detail::prevent_writing_t<range_reference_t<inner_range_type>>
            >;

    public:
        explicit intersect_iterator
            (
                outer_range_iterator first, outer_range_iterator last,
                Compare compare = Compare()
            ):
            m_begin{},
            m_end{},
            m_compare(compare)
        {
            if (std::none_of(first, last, [] (const auto & range) {return range.empty();}))
            {
                BOOST_ASSERT((std::all_of(first, last,
                    [this] (const auto & range)
                    {
                        return boost::algorithm::is_sorted(range, m_compare);
                    })));

                m_begin = std::move(first);
                m_end = std::move(last);
                std::sort(m_begin, m_end, each(front) | m_compare);
                settle();
            }
        }

        intersect_iterator (iterator::end_tag_t, const intersect_iterator & begin):
            m_begin(begin.m_begin),
            m_end(begin.m_begin),
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
            auto max_range = m_begin;
            for (auto range = m_begin; range != m_end; ++range)
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
            std::iter_swap(max_range, std::prev(m_end));
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
            if (m_begin != m_end)
            {
                const auto max_range = std::prev(m_end);

                auto range = m_begin;
                while (range != max_range)
                {
                    if (m_compare(range->front(), max_range->front()))
                    {
                        skip_to_lower_bound(*range, max_range->front(), m_compare);
                        if (range->empty())
                        {
                            scroll_to_end();
                            return;
                        }
                    }

                    if (m_compare(max_range->front(), range->front()))
                    {
                        // Возможно, тут надо продвинуть последний диапазон до нового минимума и
                        // посмотреть, нужно ли его после этого менять местами с текущим
                        // диапазоном. Возможно, это будет цикл.
                        std::iter_swap(max_range, range);
                        range = m_begin;
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
            m_end = m_begin;
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_begin->front();
        }

        bool equal (const intersect_iterator & that) const
        {
            assert(this->m_begin == that.m_begin);
            return std::equal(this->m_begin, this->m_end, that.m_begin, that.m_end);
        }

    private:
        outer_range_iterator m_begin;
        outer_range_iterator m_end;
        compare_type m_compare;
    };

    //!     Функция для создания итератора пересечения с предикатом.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь, и операцию, задающую
        отношение строгого порядка на элементах этого диапазона.
            Сами диапазоны должны быть упорядочены относительно этой операции.
            Возвращает итератор на первое пересечение входных диапазонов.
     */
    template <typename RandomAccessIterator, typename Compare>
    auto
        make_intersect_iterator
        (
            RandomAccessIterator first, RandomAccessIterator last,
            Compare compare
        )
    {
        return
            intersect_iterator<RandomAccessIterator, Compare>
            (
                std::move(first), std::move(last),
                compare
            );
    }

    template <typename RandomAccessRange, typename Compare>
    auto make_intersect_iterator (RandomAccessRange && ranges, Compare compare)
    {
        using std::begin;
        using std::end;
        return
            make_intersect_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges)),
                compare
            );
    }

    /*!
        \brief
            Функция для создания итератора пересечения с предикатом из кортежа ссылок

        \details
            Создаёт итератор ленивого пересечения нескольких диапазонов. Проход от созданного
            итератора до итератора-конца (см. перегрузку с `end_tag_t`) перечисляет те, и только те
            элементы исходных диапазонов `ranges`, которые присутствуют одновременно во всех этих
            диапазонах, причём в порядке их неубывания относительно операции `compare`.

        \param ranges
            Кортеж ссылок на диапазоны, которые нужно пересечь.
        \param compare
            Операция, задающая отношение строгого порядка на элементах результирующего диапазона.

        \pre
            Каждый диапазон в `ranges` упорядочен относительно операции `compare`.

        \returns
            Итератор на наименьший относительно заданного отношения порядка элемент ленивого
            пересечения входных диапазонов.

        \see intersect_iterator
     */
    template <typename ... Ranges, typename Compare>
    auto make_intersect_iterator (std::tuple<Ranges &...> ranges, Compare compare)
    {
        return
            make_intersect_iterator
            (
                burst::own_as_range(burst::apply(burst::make_range_vector, ranges)),
                std::move(compare)
            );
    }

    //!     Функция для создания итератора пересечения.
    /*!
            Принимает на вход набор диапазонов, которые нужно пересечь.
            Возвращает итератор на первое пересечение входных диапазонов.
            Отношение порядка для элементов диапазона выбирается по-умолчанию.
     */
    template <typename RandomAccessIterator>
    auto make_intersect_iterator (RandomAccessIterator first, RandomAccessIterator last)
    {
        return intersect_iterator<RandomAccessIterator>(std::move(first), std::move(last));
    }

    template <typename RandomAccessRange>
    auto make_intersect_iterator (RandomAccessRange && ranges)
    {
        using std::begin;
        using std::end;
        return
            make_intersect_iterator
            (
                begin(std::forward<RandomAccessRange>(ranges)),
                end(std::forward<RandomAccessRange>(ranges))
            );
    }

    /*!
        \brief
            Функция для создания итератора пересечения из кортежа ссылок

        \returns
            Итератор ленивого пересечения с отношением порядка по умолчанию.

        \see make_intersect_iterator
        \see intersect_iterator
     */
    template <typename ... Ranges>
    auto make_intersect_iterator (std::tuple<Ranges &...> ranges)
    {
        return
            make_intersect_iterator
            (
                burst::own_as_range(burst::apply(burst::make_range_vector, ranges))
            );
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
