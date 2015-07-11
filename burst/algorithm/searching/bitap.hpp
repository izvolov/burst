#ifndef BURST_ALGORITHM_SEARCHING_BITAP_HPP
#define BURST_ALGORITHM_SEARCHING_BITAP_HPP

#include <array>
#include <climits>
#include <iterator>
#include <map>
#include <memory>
#include <type_traits>

#include <boost/assert.hpp>

#include <burst/algorithm/searching/detail/element_position_bitmask_table.hpp>
#include <burst/algorithm/searching/detail/bitmask_traits.hpp>

namespace burst
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
        template
        <
            typename Value,
            typename Bitmask,
            typename Map = typename std::conditional
            <
                std::is_integral<Value>::value && sizeof(Value) == 1,
                std::array<Bitmask, (1ul << (sizeof(Value) * CHAR_BIT))>,
                std::map<Value, Bitmask>
            >
            ::type
        >
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
                m_bitmask_table(std::make_shared<const bitmask_table_type>(pattern_begin, pattern_end))
            {
                BOOST_ASSERT(m_bitmask_table->length() <= bitmask_size);
            }

            template <typename ForwardRange>
            explicit bitap (const ForwardRange & pattern):
                m_bitmask_table(std::make_shared<const bitmask_table_type>(pattern.begin(), pattern.end()))
            {
                BOOST_ASSERT(m_bitmask_table->length() <= bitmask_size);
            }

        public:
            //!     Поиск образца в тексте.
            /*!
                    Возвращает итератор на первое вхождение образца, которым проинициализирован
                данный объект, в текст, заданный полуинтервалом [corpus_begin, corpus_end).
             */
            template <typename ForwardIterator>
            ForwardIterator operator () (ForwardIterator corpus_begin, ForwardIterator corpus_end) const
            {
                typedef typename std::iterator_traits<ForwardIterator>::value_type iterated_type;
                static_assert(std::is_same<iterated_type, value_type>::value, "Тип элементов обыскиваемой последовательности должен совпадать с типом элементов образца.");

                return do_search(corpus_begin, corpus_end);
            }

        private:
            template <typename ForwardIterator>
            ForwardIterator do_search (ForwardIterator first, ForwardIterator last) const
            {
                bitmask_type match_column = 0x00;
                ForwardIterator match_candidate = first;
                first = dummy_search(first, last, match_column);
                return active_search(match_candidate, first, last, match_column);
            }

            //!     "Холостой поиск".
            /*!
                    Первое вхождение образца в текст не может произойти ранее, чем на позиции
                |P| - 1, где |P| — длина образца. Поэтому при первом поиске до тех пор, пока от
                начала обыскиваемой последовательности не пройдено расстояние, равное длине образца
                без единицы, можно просто пройти по тексту и подсчитать "подсказку" для дальнейшего
                поиска.
             */
            template <typename ForwardIterator>
            ForwardIterator dummy_search (ForwardIterator first, ForwardIterator last, bitmask_type & hint) const
            {
                std::size_t dummy_iteration_count = 0;
                while (first != last && dummy_iteration_count < m_bitmask_table->length())
                {
                    hint = bit_shift(hint) & (*m_bitmask_table)[*first];

                    ++first;
                    ++dummy_iteration_count;
                }

                return first;
            }

            //!     "Активный поиск".
            /*!
                    Принимает три итератора:
                    1. Кандидат на совпадение ("match_candidate").
                    2. Место в тексте, с которого будет продолжаться поиск ("corpus_current").
                    3. Конец текста ("corpus_end").

                    Кандидат на совпадение отстаёт от текущего места поиска ровно на |P| позиций.
                Это позволяет сразу, без дополнительных вычислений, вернуть результат поиска в том
                случае, когда найдено совпадение.
                    При этом поиск начинается сразу же с итератора "corpus_current". Это возможно
                благодаря тому, что для всех позиций в полуинтервале [match_candidate, corpus_current)
                уже посчитана маска-индикатор совпадений — "подсказка" ("hint").
             */
            template <typename ForwardIterator>
            ForwardIterator active_search (ForwardIterator match_candidate, ForwardIterator corpus_current, ForwardIterator corpus_end, bitmask_type & hint) const
            {
                // Индикатор совпадения — единица на N-м месте в битовой маске, где N — количество элементов в искомом образце.
                const bitmask_type match_indicator = static_cast<bitmask_type>(0x01u << (m_bitmask_table->length() - 1));
                bitmask_type & match_column = hint;

                while (corpus_current != corpus_end && (match_column & match_indicator) == 0)
                {
                    match_column = bit_shift(match_column) & (*m_bitmask_table)[*corpus_current];
                    ++corpus_current;
                    ++match_candidate;
                }

                return (match_column & match_indicator) == 0 ? corpus_end : match_candidate;
            }

            //!     Главная битовая операция алгоритма.
            /*!
                    Суть операции в том, что к битовой маске применяется побитовый сдвиг на одну
                позицию влево (то есть в сторону старшего бита), а затем младший бит
                устанавливается в единицу.
             */
            static bitmask_type bit_shift (bitmask_type bitmask)
            {
                bitmask = static_cast<bitmask_type>(bitmask << 1);
                bitmask_type low_bit = 0x01;

                return bitmask | low_bit;
            }

        public:
            static const std::size_t bitmask_size = detail::bitmask_traits<bitmask_type>::size;

        private:
            // Для одинаковых образцов таблицы масок также одинаковые. Поэтому на все одинаковые
            // образцы достаточно одной копии таблицы, указатель на которую можно смело копировать
            // из одного объекта в другой.
            const std::shared_ptr<const bitmask_table_type> m_bitmask_table;
        };
    } // namespace algorithm
} // namespace burst

#endif // BURST_ALGORITHM_SEARCHING_BITAP_HPP
