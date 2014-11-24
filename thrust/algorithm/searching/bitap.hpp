#ifndef THRUST_ALGORITHM_SEARCHING_BITAP_HPP
#define THRUST_ALGORITHM_SEARCHING_BITAP_HPP

#include <iterator>
#include <map>
#include <type_traits>

#include <boost/assert.hpp>

#include <thrust/algorithm/searching/detail/element_position_bitmask_table.hpp>
#include <thrust/algorithm/searching/detail/bitmask_traits.hpp>

namespace thrust
{
    namespace algorithm
    {
        //!     Двоичный алгоритм поиска подстроки (он же "bitap" или "shift-or").
        /*!
                \tparam Value
                    Тип элементов образца и элементов текста, с которыми будет происходить работа.
                \tparam Bitmask
                    Тип битовой маски. Должен поддерживать хотя бы инициализацию из целых чисел и
                    операции побитового сдвига влево (<<), И (&) и ИЛИ (|). Также требуется
                    специализация типа bitmask_traits<T> для выяснения размера битовой маски
                    (bitmask_traits<Bitmask>::size).
                \tparam Map
                    Ассоциативный контейнер, хранящий отображение элементов образца в битовые маски.
         */
        template <typename Value, typename Bitmask, typename Map = std::map<Value, Bitmask>>
        class bitap
        {
        public:
            typedef Value value_type;
            typedef Bitmask bitmask_type;
            typedef Map map_type;
            typedef detail::element_position_bitmask_table<map_type> bitmask_table_type;

        public:
            template <typename InputIterator>
            bitap (InputIterator pattern_begin, InputIterator pattern_end):
                m_bitmask_table(pattern_begin, pattern_end)
            {
                BOOST_ASSERT(m_bitmask_table.length() <= bitmask_size);
            }

            template <typename ForwardRange>
            explicit bitap (const ForwardRange & pattern):
                m_bitmask_table(pattern.begin(), pattern.end())
            {
                BOOST_ASSERT(m_bitmask_table.length() <= bitmask_size);
            }

        public:
            //!     Поиск образца в тексте.
            /*!
                    Возвращает итератор на первое вхождение образца, которым проинициализирован
                данный объект, в текст, заданный полуинтервалом [corpus_begin, corpus_end).
             */
            template <typename RandomAccessIterator>
            RandomAccessIterator operator () (RandomAccessIterator corpus_begin, RandomAccessIterator corpus_end) const
            {
                typedef typename std::iterator_traits<RandomAccessIterator>::value_type iterated_type;
                static_assert(std::is_same<iterated_type, value_type>::value, "Тип элементов обыскиваемой последовательности должен совпадать с типом элементов образца.");

                return do_search(corpus_begin, corpus_end);
            }

            //!     Поиск образца с подсказкой.
            /*!
                    Не только возвращает итератор на первое вхождение образца в текст, но и
                сохраняет в переменной "hint" результат поиска. Таким образом, при поиске
                следующего вхождения не нужно откатываться назад, а можно продолжить поиск со
                следующей позиции после последнего найденного вхождения, передав в функцию
                дополнительную "подсказку" — результат предыдущего поиска.
             */
            template <typename RandomAccessIterator>
            RandomAccessIterator operator () (RandomAccessIterator corpus_begin, RandomAccessIterator corpus_end, bitmask_type & hint) const
            {
                typedef typename std::iterator_traits<RandomAccessIterator>::value_type iterated_type;
                static_assert(std::is_same<iterated_type, value_type>::value, "Тип элементов обыскиваемой последовательности должен совпадать с типом элементов образца.");

                return do_search(corpus_begin, corpus_end, hint);
            }

        private:
            template <typename RandomAccessIterator>
            RandomAccessIterator do_search (RandomAccessIterator first, RandomAccessIterator last) const
            {
                bitmask_type match_column = 0x00;
                return do_search(first, last, match_column);
            }

            template <typename RandomAccessIterator>
            RandomAccessIterator do_search (RandomAccessIterator first, RandomAccessIterator last, bitmask_type & hint) const
            {
                // Индикатор совпадения — единица на N-м месте в битовой маске, где N — количество элементов в искомом образце.
                const bitmask_type match_indicator = static_cast<bitmask_type>(0x01 << (m_bitmask_table.length() - 1));
                bitmask_type & match_column = hint;

                while (first != last && (match_column & match_indicator) == 0)
                {
                    match_column = bit_shift(match_column) & m_bitmask_table[*first];
                    ++first;
                }

                // Если совпадение есть, то итератор first находится за его концом, поэтому нужно
                // откатиться на количество слов в образце.
                typedef typename std::iterator_traits<RandomAccessIterator>::difference_type difference_type;
                difference_type rollback = static_cast<difference_type>(m_bitmask_table.length());
                return (match_column & match_indicator) == 0 ? last : first - rollback;
            }

            //!     Главная битовая операция алгоритма.
            /*!
                    Суть операции в том, что к битовой маске применяется побитовый сдвиг на одну
                позицию влево (то есть в сторону старшего бита), а затем младший бит
                устанавливается в единицу.
             */
            static bitmask_type bit_shift (bitmask_type bitmask)
            {
                bitmask <<= 1;
                bitmask_type low_bit = 0x01;

                return bitmask | low_bit;
            }

        public:
            static const std::size_t bitmask_size = detail::bitmask_traits<bitmask_type>::size;

        private:
            bitmask_table_type m_bitmask_table;
        };
    } // namespace algorithm
} // namespace thrust

#endif // THRUST_ALGORITHM_SEARCHING_BITAP_HPP
