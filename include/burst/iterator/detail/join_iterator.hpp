#ifndef BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP
#define BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/type_traits/iterator_reference.hpp>
#include <burst/type_traits/iterator_value.hpp>
#include <burst/type_traits/range_iterator.hpp>
#include <burst/type_traits/range_reference.hpp>
#include <burst/type_traits/range_value.hpp>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/next_prior.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iterator>
#include <numeric>
#include <type_traits>
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
                range_value_t<iterator_value_t<InputIterator>>,
                boost::single_pass_traversal_tag,
                range_reference_t<iterator_value_t<InputIterator>>
            >
        {
        private:
            using outer_range_iterator = InputIterator;
            using inner_range_type = iterator_value_t<outer_range_iterator>;
            static_assert
            (
                std::is_lvalue_reference<iterator_reference_t<outer_range_iterator>>::value,
                ""
            );

            using base_type =
                boost::iterator_facade
                <
                    join_iterator_impl,
                    range_value_t<inner_range_type>,
                    boost::single_pass_traversal_tag,
                    range_reference_t<inner_range_type>
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
            хранится в неизменном виде, а текущая позиция в склеенном диапазоне задаётся двумя
            итераторами: во внешнем и внутреннем диапазонах.
         */
        template <typename RandomAccessIterator>
        class join_iterator_impl<RandomAccessIterator, boost::random_access_traversal_tag>:
            public boost::iterator_facade
            <
                join_iterator_impl<RandomAccessIterator, boost::random_access_traversal_tag>,
                range_value_t<iterator_value_t<RandomAccessIterator>>,
                boost::random_access_traversal_tag,
                range_reference_t<iterator_value_t<RandomAccessIterator>>
            >
        {
        private:
            using outer_range_iterator = RandomAccessIterator;
            using inner_range_type = iterator_value_t<outer_range_iterator>;
            using inner_range_iterator = range_iterator_t<inner_range_type>;

            using base_type =
                boost::iterator_facade
                <
                    join_iterator_impl,
                    range_value_t<inner_range_type>,
                    boost::random_access_traversal_tag,
                    range_reference_t<inner_range_type>
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
                m_begin(std::move(first)),
                m_end(std::move(last)),
                m_outer(m_begin != m_end ? m_begin : outer_range_iterator{}),
                m_inner(m_begin != m_end ? m_outer->begin() : inner_range_iterator{}),
                m_items_remaining{}
            {
                if (m_begin != m_end)
                {
                    m_items_remaining =
                        std::accumulate(m_begin, m_end, 0u,
                            [] (auto v, const auto & r)
                            {
                                return v + r.size();
                            });

                    maintain_invariant();
                }
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
                m_begin(begin.m_begin),
                m_end(begin.m_end),
                m_outer(m_begin != m_end ? m_end : outer_range_iterator{}),
                m_inner(m_begin != m_end ? boost::prior(m_end)->end() : inner_range_iterator{}),
                m_items_remaining(0)
            {
            }

            join_iterator_impl () = default;

        private:
            friend class boost::iterator_core_access;

            //!     Поддержать инвариант.
            /*!
                    Итераторы всегда установлены либо на некоторый элемент непустого диапазона,
                либо за последним элементом последнего диапазона, а именно: внешний итератор
                указывает на конец внешних диапазонов, а внутренний итератор — на конец последнего
                из внешних диапазонов.
             */
            void maintain_invariant ()
            {
                while (m_inner == m_outer->end() &&
                    ++m_outer != m_end)
                {
                    m_inner = m_outer->begin();
                }
            }

            //!     Подготовить итераторы к сдвигу назад
            /*!
                    Нужно произвести операцию, в некотором смысле обратную к `maintain_invariant`:
                    Если итераторы указывают на конец, то внешний итератор нужно сдвинуть на одну
                позицию влево. Далее нужно привести итераторы в такое положение, что будет доступен
                сдвиг хотя бы на один элемент влево (с точки зрения всего итератора склейки).
             */
            void prepare_for_decrement ()
            {
                if (m_outer == m_end)
                {
                    --m_outer;
                }
                while (m_inner == m_outer->begin())
                {
                    --m_outer;
                    m_inner = m_outer->end();
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
                    auto items_remaining_in_current_range = std::distance(m_inner, m_outer->end());
                    auto items_to_scroll_in_current_range = std::min(n, items_remaining_in_current_range);
                    n -= items_to_scroll_in_current_range;
                    m_inner += items_to_scroll_in_current_range;

                    maintain_invariant();
                }
            }

            //!     Назад на n элементов.
            void backward (typename base_type::difference_type n)
            {
                while (n > 0)
                {
                    prepare_for_decrement();

                    auto items_remaining_in_current_range = std::distance(m_outer->begin(), m_inner);
                    auto items_to_scroll_in_current_range = std::min(n, items_remaining_in_current_range);
                    n -= items_to_scroll_in_current_range;
                    m_inner -= items_to_scroll_in_current_range;
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
                ++m_inner;
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
                prepare_for_decrement();
                --m_inner;
                ++m_items_remaining;
            }

        private:
            typename base_type::reference dereference () const
            {
                return *m_inner;
            }

            bool equal (const join_iterator_impl & that) const
            {
                assert(this->m_begin == that.m_begin);
                assert(this->m_end == that.m_end);
                return this->m_items_remaining == that.m_items_remaining;
            }

            typename base_type::difference_type distance_to (const join_iterator_impl & that) const
            {
                return this->m_items_remaining - that.m_items_remaining;
            }

        private:
            outer_range_iterator m_begin;
            outer_range_iterator m_end;

            outer_range_iterator m_outer;
            inner_range_iterator m_inner;

            typename base_type::difference_type m_items_remaining;
        };
    }
}

#endif // BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP
