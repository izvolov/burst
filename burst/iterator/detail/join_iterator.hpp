#ifndef BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP
#define BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP

#include <burst/iterator/end_tag.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/difference_type.hpp>
#include <boost/range/distance.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/reference.hpp>
#include <boost/range/value_type.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iterator>
#include <utility>

namespace burst
{
    namespace detail
    {
        //!     Однопроходный итератор склейки.
        /*!
                Специализация итератора склейки для того случая, когда склеиваемые диапазоны не
            являются диапазонами произвольного доступа.
         */
        template <typename InputIterator, typename IteratorCategory>
        class join_iterator_impl:
            public boost::iterator_facade
            <
                join_iterator_impl<InputIterator, IteratorCategory>,
                typename boost::range_value<typename std::iterator_traits<InputIterator>::value_type>::type,
                boost::single_pass_traversal_tag,
                typename boost::range_reference<typename std::iterator_traits<InputIterator>::value_type>::type
            >
        {
        private:
            using outer_range_iterator = InputIterator;
            using inner_range_type = typename std::iterator_traits<outer_range_iterator>::value_type;
            static_assert
            (
                std::is_lvalue_reference
                <
                    typename std::iterator_traits<outer_range_iterator>::reference
                >
                ::value,
                ""
            );

            using base_type =
                boost::iterator_facade
                <
                    join_iterator_impl,
                    typename boost::range_value<inner_range_type>::type,
                    boost::single_pass_traversal_tag,
                    typename boost::range_reference<inner_range_type>::type
                >;

        public:
            //!     Создание итератора на начало склеенного диапазона.
            /*!
                    Принимает диапазон диапазонов, которые нужно склеить, и копирует его себе
                внутрь. Затем продвигает внешний диапазон так, чтобы поддерживался инвариант.

                    Асимптотика.

                Время: O(|R|),
                    |R| — количество склеиваемых диапазонов.

                Память: O(1).
             */
            explicit join_iterator_impl (outer_range_iterator first, outer_range_iterator last):
                m_begin(std::move(first)),
                m_end(std::move(last))
            {
                maintain_invariant();
            }

            //!     Создание итератора на конец склеенного диапазона.
            /*!
                    Принимает итератор на начала склеенного диапазона и метку, обозначающую, что
                нужно создать именно итератор-конец.
                    Ничего не делает, входные аргументы игнорирует.

                    Асимптотика.

                Время: O(1).
                Память: O(1).
             */
            join_iterator_impl (iterator::end_tag_t, const join_iterator_impl & begin):
                m_begin(begin.m_end),
                m_end(begin.m_end)
            {
            }

            join_iterator_impl () = default;

        private:
            friend class boost::iterator_core_access;

            //!     Поддержать инвариант.
            /*!
                    Инвариант состоит в том, что либо внешний диапазон пуст, либо первый из
                внутренних диапазонов непуст.

                    Асимптотика.

                Время: O(|R|),
                    |R| — количество склеиваемых диапазонов.
                    Худший случай достигается тогда, когда все внутренние диапазоны пусты.

                Память: O(1).
             */
            void maintain_invariant ()
            {
                m_begin =
                    std::find_if_not(m_begin, m_end,
                        [] (const auto & r)
                        {
                            return r.empty();
                        });
            }

            //!     Продвижение итератора на один элемент вперёд.
            /*!
                    Продвигает текущий из склеиваемых диапазонов на один элемент вперёд, а затем
                поддерживает инвариант.

                    Асимптотика.

                Время: O(|R|),
                    |R| — количество склеиваемых диапазонов.

                Память: O(1).
             */
            void increment ()
            {
                m_begin->advance_begin(1);
                maintain_invariant();
            }

        private:
            typename base_type::reference dereference () const
            {
                return m_begin->front();
            }

            bool equal (const join_iterator_impl & that) const
            {
                return
                    (this->m_begin == this->m_end && that.m_begin == that.m_end) ||
                    (this->m_begin != this->m_end && that.m_begin != that.m_end &&
                        this->m_begin == that.m_begin &&
                        this->m_begin->begin() == that.m_begin->begin());
            }

        private:
            outer_range_iterator m_begin;
            outer_range_iterator m_end;
        };

        //!     Итератор склейки произвольного доступа.
        /*!
                Специализация итератора склейки для того случая, когда склеиваемые диапазоны
            являются диапазонами произвольного доступа.
                Особенностью этой специализации является то, что склеиваемый диапазон диапазонов
            хранится в буфере в неизменном виде, а текущая позиция в склеенном диапазоне задаётся
            двумя индексами: индексом во внешнем и внутреннем диапазонах.
         */
        template <typename RandomAccessIterator>
        class join_iterator_impl<RandomAccessIterator, boost::random_access_traversal_tag>:
            public boost::iterator_facade
            <
                join_iterator_impl<RandomAccessIterator, boost::random_access_traversal_tag>,
                typename boost::range_value<typename std::iterator_traits<RandomAccessIterator>::value_type>::type,
                boost::random_access_traversal_tag,
                typename boost::range_reference<typename std::iterator_traits<RandomAccessIterator>::value_type>::type
            >
        {
        private:
            using outer_range_iterator = RandomAccessIterator;
            using inner_range_type = typename std::iterator_traits<outer_range_iterator>::value_type;

            using outer_difference = typename std::iterator_traits<outer_range_iterator>::difference_type;
            using inner_difference = typename boost::range_difference<inner_range_type>::type;

            using base_type =
                boost::iterator_facade
                <
                    join_iterator_impl,
                    typename boost::range_value<inner_range_type>::type,
                    boost::random_access_traversal_tag,
                    typename boost::range_reference<inner_range_type>::type
                >;

        public:
            //!     Создание итератора на начало склеенного диапазона.
            /*!
                    Копирует диапазон диапазонов, а также устанавливает позиционирующие индексы в
                ноль и подсчитывает количество оставшихся элементов, равное суммарному размеру
                входных диапазонов.

                    Асимптотика.

                Время: O(|R|).
                Память: O(1).
             */
            explicit join_iterator_impl (outer_range_iterator first, outer_range_iterator last):
                m_ranges(std::move(first), std::move(last)),
                m_outer_range_index(0),
                m_inner_range_index(0),
                m_items_remaining(0)
            {
                using boost::adaptors::transformed;
                const auto to_size = [] (const auto & r) { return r.size(); };
                m_items_remaining = boost::accumulate(m_ranges | transformed(to_size), 0u);

                maintain_invariant();
            }

            //!     Создание итератора на конец склеенного диапазона.
            /*!
                    Принимает итератор на начало склеенного диапазона и метку, обозначающую, что
                нужно создать именно итератор-конец.
                    Берёт из входного итератора указатель на буфер, устанавливает индекс текущего
                диапазона на конец буфера, а индекс элемента в буфере на ноль. Также устанавливает
                счётчик оставшихся элементов в ноль.

                    Асимптотика.

                Время: O(1).
                Память: O(1).
             */
            join_iterator_impl (iterator::end_tag_t, const join_iterator_impl & begin):
                m_ranges(begin.m_ranges),
                m_outer_range_index(boost::distance(m_ranges)),
                m_inner_range_index(0),
                m_items_remaining(0)
            {
            }

            join_iterator_impl () = default;

        private:
            friend class boost::iterator_core_access;

            //!     Поддержать инвариант.
            /*!
                    Индексы всегда установлены либо на первый элемент непустого диапазона, либо за
                последним элементом последнего диапазона.
             */
            void maintain_invariant ()
            {
                while (m_outer_range_index < boost::distance(m_ranges) &&
                    m_inner_range_index == boost::distance(m_ranges[m_outer_range_index]))
                {
                    ++m_outer_range_index;
                    m_inner_range_index = 0;
                }
            }

            //!     Продвижение итератора на несколько позиций сразу.
            /*!
                    Продвигает индекс элемента в текущем диапазоне на столько позиций из требуемого
                количества позиций, сколько позволяет размер этого диапазона. Если индекс дошёл до
                конца (или начала — в зависимости от того, в какую сторону происходит продвижение)
                текущего диапазона, и при этом пройдено не всё требуемое количество позиций, то
                переходит к следующему по порядку диапазону, смотрит, сколько можно пройти в нём и
                т.д.

                    Асимптотика.

                Время: O(|R|).
                    При коротких склеиваемых диапазонах вырождается в O(n), а при длинных,
                    наоборот, стремится к O(1).

                Память: O(1).
             */
            void advance (typename base_type::difference_type n)
            {
                m_items_remaining -= n;

                if (n > 0)
                {
                    forward(std::abs(n));
                }
                else
                {
                    backward(std::abs(n));
                }
            }

            //!     Вперёд на n элементов.
            void forward (typename base_type::difference_type n)
            {
                while (n > 0)
                {
                    auto items_remaining_in_current_range =
                        boost::distance(m_ranges[m_outer_range_index]) - m_inner_range_index;
                    auto items_to_scroll_in_current_range = std::min(n, items_remaining_in_current_range);
                    n -= items_to_scroll_in_current_range;

                    m_inner_range_index += items_to_scroll_in_current_range;
                    maintain_invariant();
                }
            }

            //!     Назад на n элементов.
            void backward (typename base_type::difference_type n)
            {
                while (n > 0)
                {
                    if (m_inner_range_index == 0)
                    {
                        --m_outer_range_index;
                        m_inner_range_index = boost::distance(m_ranges[m_outer_range_index]);
                    }

                    auto items_remaining_in_current_range = m_inner_range_index;
                    auto items_to_scroll_in_current_range = std::min(n, items_remaining_in_current_range);
                    n -= items_to_scroll_in_current_range;
                    m_inner_range_index -= items_to_scroll_in_current_range;
                }
            }

            //!     Продвижение итератора на один элемент вперёд.
            /*!
                    Увеличивает индекс в текущем диапазоне на единицу. Если индекс попал в конец
                текущего диапазона, то увеличивает индекс в буфере диапазонов на один, а индекс
                текущего элемента устанавливает в ноль.
                    Затем поддерживает инвариант.

                    Асимптотика.

                Время: O(|R|).
                Память: O(1).
             */
            void increment ()
            {
                ++m_inner_range_index;
                maintain_invariant();
                --m_items_remaining;
            }

            //!     Продвижение итератора на один элемент назад.
            /*!
                    Аналогичен продвижению вперёд, только двигается в обратном направлении.

                    Асимптотика.

                Время: O(1).
                Память: O(1).
             */
            void decrement ()
            {
                while (m_inner_range_index == 0)
                {
                    --m_outer_range_index;
                    m_inner_range_index = boost::distance(m_ranges[m_outer_range_index]);
                }

                --m_inner_range_index;
                ++m_items_remaining;
            }

        private:
            typename base_type::reference dereference () const
            {
                return m_ranges[m_outer_range_index][m_inner_range_index];
            }

            bool equal (const join_iterator_impl & that) const
            {
                assert(this->m_ranges == that.m_ranges);
                return this->m_items_remaining == that.m_items_remaining;
            }

            typename base_type::difference_type distance_to (const join_iterator_impl & that) const
            {
                return this->m_items_remaining - that.m_items_remaining;
            }

        private:
            boost::iterator_range<outer_range_iterator> m_ranges;

            outer_difference m_outer_range_index;
            inner_difference m_inner_range_index;

            typename base_type::difference_type m_items_remaining;
        };
    }
}

#endif // BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP
