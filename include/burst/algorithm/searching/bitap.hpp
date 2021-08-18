#ifndef BURST__ALGORITHM__SEARCHING__BITAP_HPP
#define BURST__ALGORITHM__SEARCHING__BITAP_HPP

#include <burst/algorithm/searching/detail/bitmask_traits.hpp>
#include <burst/algorithm/searching/detail/element_position_bitmask_table.hpp>
#include <burst/integer/left_shift.hpp>

#include <boost/assert.hpp>
#include <boost/range/iterator_range.hpp>

#include <array>
#include <climits>
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <type_traits>

#define ITERATED_TYPE_MUST_MATCH_SEARCHED_TYPE__ERROR_MESSAGE \
"Тип элементов обыскиваемой последовательности должен совпадать с типом элементов образца."

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
                std::array<Bitmask, (1ul << CHAR_BIT)>,
                std::map<Value, Bitmask>
            >
            ::type
        >
        class bitap
        {
        public:
            using value_type = Value;
            using bitmask_type = Bitmask;
            using map_type = Map;
            using bitmask_table_type = detail::element_position_bitmask_table<map_type>;

        public:
            template <typename InputIterator>
            bitap (InputIterator pattern_begin, InputIterator pattern_end):
                m_bitmask_table
                (
                    std::make_shared<const bitmask_table_type>(pattern_begin, pattern_end)
                )
            {
                BOOST_ASSERT(m_bitmask_table->length() <= bitmask_size);
            }

            template <typename ForwardRange>
            explicit bitap (const ForwardRange & pattern):
                m_bitmask_table
                (
                    std::make_shared<const bitmask_table_type>(pattern.begin(), pattern.end())
                )
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
            ForwardIterator
                operator () (ForwardIterator corpus_begin, ForwardIterator corpus_end) const
            {
                using iterated_type = iterator_value_t<ForwardIterator>;
                static_assert
                (
                    std::is_same<iterated_type, value_type>::value,
                    ITERATED_TYPE_MUST_MATCH_SEARCHED_TYPE__ERROR_MESSAGE
                );

                return do_search(corpus_begin, corpus_end);
            }

            //!     Найти первое вхождение образца в текст.
            /*!
                    Принимает диапазон, в котором нужно произвести поиск, а также ссылку на битовую
                маску — "подсказку" — которую в дальнейшем можно использовать для продолжения
                поиска с конца предыдущего вхождения.
                    Если образец не найден, то состояние "подсказки" не оговорено, то есть может
                быть любым.
             */
            template <typename ForwardIterator>
            boost::iterator_range<ForwardIterator>
                find_first
                (
                    ForwardIterator corpus_begin,
                    ForwardIterator corpus_end,
                    bitmask_type & hint
                ) const
            {
                return
                    active_search
                    (
                        corpus_begin,
                        dummy_search(corpus_begin, corpus_end, hint),
                        corpus_end,
                        hint
                    );
            }

            //!     Найти следующее вхождение образца в текст.
            /*!
                    Принимает предыдущее вхождение образца в текст, конец текста, а также
                "подсказку", в которой сохранена информация о предыдущем поиске. Используя эту
                информацию можно продолжить поиск с того места, на котором он был закончен в
                прошлый раз, вместо того, чтобы начинать поиск с элемента, следующего за началом
                предыдущего совпадения.
                    Если образец не найден, то состояние "подсказки" не оговорено, то есть может
                быть любым.
             */
            template <typename ForwardIterator>
            boost::iterator_range<ForwardIterator>
                find_next
                (
                    ForwardIterator previous_match_begin,
                    ForwardIterator previous_match_end,
                    ForwardIterator corpus_end,
                    bitmask_type & hint
                ) const
            {
                if (previous_match_end != corpus_end)
                {
                    hint = bit_shift(hint) & (*m_bitmask_table)[*previous_match_end];
                    ++previous_match_begin;
                    ++previous_match_end;

                    return
                        active_search(previous_match_begin, previous_match_end, corpus_end, hint);
                }
                else
                {
                    return boost::make_iterator_range(corpus_end, corpus_end);
                }
            }

        private:
            template <typename ForwardIterator>
            ForwardIterator do_search (ForwardIterator first, ForwardIterator last) const
            {
                auto match_column = bitmask_type{0b0};
                auto match_candidate = first;
                first = dummy_search(first, last, match_column);
                return active_search(match_candidate, first, last, match_column).begin();
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
            ForwardIterator
                dummy_search
                (
                    ForwardIterator first,
                    ForwardIterator last,
                    bitmask_type & hint
                ) const
            {
                auto dummy_iteration_count = std::size_t{0};
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
            boost::iterator_range<ForwardIterator>
                active_search
                (
                    ForwardIterator match_candidate,
                    ForwardIterator corpus_current,
                    ForwardIterator corpus_end,
                    bitmask_type & hint
                ) const
            {
                // Индикатор совпадения — единица на N-м месте в битовой маске,
                // где N — количество элементов в искомом образце.
                const auto match_indicator =
                    left_shift(bitmask_type{0b1}, m_bitmask_table->length() - 1u);
                auto & match_column = hint;

                while (corpus_current != corpus_end && (match_column & match_indicator) == 0)
                {
                    match_column = bit_shift(match_column) & (*m_bitmask_table)[*corpus_current];
                    ++corpus_current;
                    ++match_candidate;
                }

                return (match_column & match_indicator) == 0
                    ? boost::make_iterator_range(corpus_end, corpus_end)
                    : boost::make_iterator_range(match_candidate, corpus_current);
            }

            //!     Главная битовая операция алгоритма.
            /*!
                    Суть операции в том, что к битовой маске применяется побитовый сдвиг на одну
                позицию влево (то есть в сторону старшего бита), а затем младший бит
                устанавливается в единицу.
             */
            static bitmask_type bit_shift (bitmask_type bitmask)
            {
                return left_shift(bitmask, 1u) | bitmask_type{0b1};
            }

        public:
            static const std::size_t bitmask_size = detail::bitmask_traits<bitmask_type>::size;

        private:
            // Для одинаковых образцов таблицы масок также одинаковые. Поэтому на все одинаковые
            // образцы достаточно одной копии таблицы, указатель на которую можно смело копировать
            // из одного объекта в другой.
            const std::shared_ptr<const bitmask_table_type> m_bitmask_table;
        };

        //!     Функция создания поискового объекта.
        /*!
                Принимает явно заданный аргумент шаблона "Bitmask", обозначающий тип битовой маски,
            которым будет кодироваться образец, а также сам образец в виде диапазона произвольных
            элементов.
         */
        template <typename Bitmask, typename ForwardRange>
        auto make_bitap (const ForwardRange & pattern)
        {
            return bitap<typename ForwardRange::value_type, Bitmask>(pattern);
        }

        //!     Функция создания поискового объекта с произвольным отображением.
        /*!
                Отличается тем, что принимает явно заданный аргумент шаблона "Map", обозначающий
            тип, в котором будет храниться отображение из элементов образца в битовые маски.
         */
        template <typename Bitmask, typename Map, typename ForwardRange>
        auto make_bitap (const ForwardRange & pattern)
        {
            return bitap<typename ForwardRange::value_type, Bitmask, Map>(pattern);
        }
    } // namespace algorithm
} // namespace burst

#endif // BURST__ALGORITHM__SEARCHING__BITAP_HPP
