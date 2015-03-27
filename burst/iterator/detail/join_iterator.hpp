#ifndef BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP
#define BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/bind.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/numeric.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <burst/iterator/end_tag.hpp>

namespace burst
{
    namespace detail
    {
        //!     Однонаправленный итератор склейки.
        /*!
                Специализация итератора склейки для того случая, когда склеиваемые диапазоны не
            являются диапазонами произвольного доступа.
         */
        template <typename ForwardRange, typename IteratorCategory>
        class join_iterator_impl:
            public boost::iterator_facade
            <
                join_iterator_impl<ForwardRange, IteratorCategory>,
                typename ForwardRange::value_type,
                boost::forward_traversal_tag,
                typename ForwardRange::reference
            >
        {
        private:
            typedef ForwardRange range_type;

            typedef boost::iterator_facade
            <
                join_iterator_impl,
                typename range_type::value_type,
                boost::forward_traversal_tag,
                typename range_type::reference
            >
            base_type;

        public:
            //!     Создание итератора на начало склеенного диапазона.
            /*!
                    Принимает последовательность диапазонов, которые нужно склеить, и копирует
                непустые диапазоны во внутренний буфер.
                    Склейка будет происходить в том порядке, в котором диапазоны перечислены в
                последовательности. Иначе говоря, в склеенном диапазоне вначале будут все элементы
                первой последовательности, затем все элементы второй, и т.д:

                    join(R_1, ..., R_n) = R_1[1], ..., R_1[len_1], ..., R_n[1], ..., R_n[len_n]

                где R_i — i-й склеиваемый диапазон, len_i — длина i-го слеиваемого диапазона.

                    Асимптотика.

                Время: O(|R|),
                    |R| — количество склеиваемых диапазонов.

                Память: O(1).
                    Учитывается только собственная память конструктора. Сам созданный объект хранит
                    весь набор склеиваемых диапазонов.
             */
            template <typename BidirectionalRange>
            explicit join_iterator_impl (const BidirectionalRange & ranges):
                m_ranges()
            {
                BOOST_STATIC_ASSERT(boost::is_same<typename BidirectionalRange::value_type, range_type>::value);
                boost::algorithm::copy_if
                (
                    boost::adaptors::reverse(ranges),
                    std::back_inserter(m_ranges),
                    not boost::bind(&range_type::empty, _1)
                );
            }

            //!     Создание итератора на конец склеенного диапазона.
            /*!
                    Принимает последовательность диапазонов, которые нужно склеить, и метку,
                обозначающую, что нужно создать именно итератор-конец.
                    Ничего не делает, входные аргументы игнорирует.

                    Асимптотика.

                Время: O(1).
                Память: O(1).
             */
            template <typename BidirectionalRange>
            join_iterator_impl (const BidirectionalRange &, iterator::end_tag_t):
                m_ranges()
            {
            }

            join_iterator_impl () = default;

        private:
            friend class boost::iterator_core_access;

            //!     Продвижение итератора на один элемент вперёд.
            /*!
                    Продвигает текущий из склеиваемых диапазонов на один элемент вперёд. Если тот
                опустел, то выбрасывает его и переходит к следующему по порядку диапазону.

                    Асимптотика.

                Время: O(move) + O(drop),
                    O(move) — время продвижения хранимого диапазона на единицу вперёд.
                    O(drop) — время выбрасывания опустевшего диапазона из буфера. В настоящий
                    момент буфером является std::vector, поэтому эта операция занимает O(1).
                    Соответственно, если продвижение хранимого диапазона происходит за O(1), то и
                    вся операция выполняется за O(1).

                Память: O(1).
             */
            void increment ()
            {
                m_ranges.back().advance_begin(1);
                if (m_ranges.back().empty())
                {
                    m_ranges.pop_back();
                }
            }

        private:
            typename base_type::reference dereference () const
            {
                return m_ranges.back().front();
            }

            bool equal (const join_iterator_impl & that) const
            {
                return this->m_ranges == that.m_ranges;
            }

        private:
            std::vector<range_type> m_ranges;
        };

        //!     Итератор склейки произвольного доступа.
        /*!
                Специализация итератора склейки для того случая, когда склеиваемые диапазоны
            являются диапазонами произвольного доступа.
                Особенностью этой специализации является то, что склеиваемые диапазоны хранятся в
            буфере в неизменном виде, а текущая позиция в склеенном диапазоне задаётся двумя
            индексами: индексом текущего диапазона в буфере и индексом элемента в текущем
            диапазоне.
                Также буфер хранится не по значению, а по указателю, что позволяет копировать
            итератор склейки произвольного доступа за O(1).
         */
        template <typename RandomAccessRange>
        class join_iterator_impl<RandomAccessRange, std::random_access_iterator_tag>:
            public boost::iterator_facade
            <
                join_iterator_impl<RandomAccessRange, std::random_access_iterator_tag>,
                typename RandomAccessRange::value_type,
                boost::random_access_traversal_tag,
                typename RandomAccessRange::reference
            >
        {
        private:
            typedef RandomAccessRange range_type;
            typedef std::vector<range_type> range_container_type;

            typedef boost::iterator_facade
            <
                join_iterator_impl,
                typename range_type::value_type,
                boost::forward_traversal_tag,
                typename range_type::reference
            >
            base_type;

        public:
            //!     Создание итератора на начало склеенного диапазона.
            /*!
                    Копирует непустые диапазоны во внутренний буфер, а также устанавливает
                позиционирующие индексы и количество пройденных элементов в ноль.

                    Асимптотика.

                Время: O(|R|).
                Память: O(1).
             */
            template <typename InputRange>
            explicit join_iterator_impl (const InputRange & ranges):
                m_ranges(std::make_shared<range_container_type>()),
                m_outer_range_index(0),
                m_inner_range_index(0),
                m_items_passed(0)
            {
                BOOST_STATIC_ASSERT(boost::is_same<typename InputRange::value_type, range_type>::value);
                boost::algorithm::copy_if(ranges, std::back_inserter(*m_ranges), not boost::bind(&range_type::empty, _1));
            }

            //!     Создание итератора на конец склеенного диапазона.
            /*!
                    Принимает последовательность диапазонов, которые нужно склеить, и метку,
                обозначающую, что нужно создать именно итератор-конец.
                    Копирует непустые диапазоны во внутренний буфер, устанавливает индекс текущего
                диапазона на конец буфера, а индекс элемента в буфере на ноль. Также устанавливает
                счётчик пройденных элементов равным суммарному размеру входных диапазонов.

                    Асимптотика.

                Время: O(|R|).
                Память: O(1).
             */
            template <typename InputRange>
            explicit join_iterator_impl (const InputRange & ranges, iterator::end_tag_t):
                m_ranges(std::make_shared<range_container_type>()),
                m_outer_range_index(0),
                m_inner_range_index(0),
                m_items_passed(0)
            {
                BOOST_STATIC_ASSERT(boost::is_same<typename InputRange::value_type, range_type>::value);
                boost::algorithm::copy_if(ranges, std::back_inserter(*m_ranges), not boost::bind(&range_type::empty, _1));
                m_items_passed = boost::accumulate
                (
                    *m_ranges,
                    0u,
                    boost::bind(std::plus<typename range_type::size_type>(), _1, boost::bind(&range_type::size, _2))
                );
                m_outer_range_index = m_ranges->size();
            }

            join_iterator_impl () = default;

        private:
            friend class boost::iterator_core_access;

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
                auto abs_n = static_cast<typename range_type::size_type>(std::abs(n));
                m_items_passed += static_cast<typename range_type::size_type>(n);

                if (n > 0)
                {
                    forward(abs_n);
                }
                else
                {
                    backward(abs_n);
                }
            }

            //!     Вперёд на n элементов.
            void forward (typename range_type::size_type n)
            {
                while (n > 0)
                {
                    auto items_remaining_in_current_range = (*m_ranges)[m_outer_range_index].size() - m_inner_range_index;
                    auto items_to_scroll_in_current_range = std::min(n, items_remaining_in_current_range);
                    n -= items_to_scroll_in_current_range;

                    m_inner_range_index += items_to_scroll_in_current_range;
                    if (m_inner_range_index == (*m_ranges)[m_outer_range_index].size())
                    {
                        ++m_outer_range_index;
                        m_inner_range_index = 0;
                    }
                }
            }

            //!     Назад на n элементов.
            void backward (typename range_type::size_type n)
            {
                while (n > 0)
                {
                    if (m_inner_range_index == 0)
                    {
                        --m_outer_range_index;
                        m_inner_range_index = (*m_ranges)[m_outer_range_index].size() - 1;
                        --n;
                    }
                    else
                    {
                        auto items_remaining_in_current_range = m_inner_range_index;
                        auto items_to_scroll_in_current_range = std::min(n, items_remaining_in_current_range);
                        n -= items_to_scroll_in_current_range;
                        m_inner_range_index -= items_to_scroll_in_current_range;
                    }
                }
            }

            //!     Продвижение итератора на один элемент вперёд.
            /*!
                    Увеличивает индекс в текущем диапазоне на единицу. Если индекс попал в конец
                текущего диапазона, то увеличивает индекс в буфере диапазонов на один, а индекс
                текущего элемента устанавливает в ноль.

                    Асимптотика.

                Время: O(1).
                Память: O(1).
             */
            void increment ()
            {
                ++m_inner_range_index;
                if (m_inner_range_index == (*m_ranges)[m_outer_range_index].size())
                {
                    ++m_outer_range_index;
                    m_inner_range_index = 0;
                }

                ++m_items_passed;
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
                if (m_inner_range_index == 0)
                {
                    --m_outer_range_index;
                    m_inner_range_index = (*m_ranges)[m_outer_range_index].size() - 1;
                }
                else
                {
                    --m_inner_range_index;
                }

                --m_items_passed;
            }

        private:
            typename base_type::reference dereference () const
            {
                auto inner_range_index = static_cast<typename base_type::difference_type>(m_inner_range_index);
                return (*m_ranges)[m_outer_range_index][inner_range_index];
            }

            bool equal (const join_iterator_impl & that) const
            {
                return this->m_items_passed == that.m_items_passed;
            }

            typename base_type::difference_type distance_to (const join_iterator_impl & that) const
            {
                return static_cast<typename base_type::difference_type>(that.m_items_passed - this->m_items_passed);
            }

        private:
            std::shared_ptr<range_container_type> m_ranges;

            typename std::vector<range_type>::size_type m_outer_range_index;
            typename range_type::size_type m_inner_range_index;

            typename range_type::size_type m_items_passed;
        };
    }
}

#endif // BURST_ITERATOR_DETAIL_JOIN_ITERATOR_HPP
