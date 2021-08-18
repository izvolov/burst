#ifndef BURST__ALGORITHM__SEARCHING__DETAIL__ELEMENT_POSITION_BITMASK_TABLE_HPP
#define BURST__ALGORITHM__SEARCHING__DETAIL__ELEMENT_POSITION_BITMASK_TABLE_HPP

#include <burst/integer/left_shift.hpp>
#include <burst/type_traits/iterator_value.hpp>

#include <array>
#include <cstddef>
#include <initializer_list>
#include <type_traits>

namespace burst
{
    namespace algorithm
    {
        namespace detail
        {
            //!     Позиционные маски элементов последовательности.
            /*!
                    Запоминает информацию о положении элементов последовательности: для каждого
                уникального элемента последовательности хранит битовую маску, в которой стоят
                единицы на тех местах, на которых элемент встречается в последовательности.
             */
            template <typename Map>
            class element_position_bitmask_table
            {
            public:
                using map_type = Map;
                using key_type = typename map_type::key_type;
                using bitmask_type = typename map_type::mapped_type;

            public:
                template <typename InputIterator>
                element_position_bitmask_table
                (
                    InputIterator sequence_begin,
                    InputIterator sequence_end
                )
                {
                    initialize(sequence_begin, sequence_end);
                }

                template <typename InputRange>
                explicit element_position_bitmask_table (const InputRange & sequence)
                {
                    initialize(sequence.begin(), sequence.end());
                }

                explicit element_position_bitmask_table (std::initializer_list<key_type> sequence)
                {
                    initialize(sequence.begin(), sequence.end());
                }

            private:
                template <typename InputIterator>
                void initialize (InputIterator first, InputIterator last)
                {
                    using iterated_type = iterator_value_t<InputIterator>;
                    static_assert(std::is_same<iterated_type, key_type>::value,
                        "Неверно задан тип входного элемента.");

                    auto elements_count = std::size_t{0};

                    auto position_indicator = bitmask_type{0b1};
                    while (first != last)
                    {
                        m_bitmasks[*first] |= position_indicator;

                        ++first;
                        position_indicator = left_shift(position_indicator, 1u);
                        ++elements_count;
                    }

                    m_sequence_length = elements_count;
                }

            public:
                //!     Взять битовую маску для элемента.
                /*!
                        Возвращает битовую маску положений заданного элемента в исходной
                    последовательности. Если элемент был в исходной последовательности, то в маске
                    будут стоять единицы на соответствующих местах (нумерация элементов в битовой
                    маске начинается с младшего бита). Если же в исходной последовательности не
                    было заданного элемента, то результирующая маска будет полностью нулевой.
                        Например, если исходная последовательность — abac, то
                        bitmask_table[a] == 0b0101
                        bitmask_table[b] == 0b0010
                        bitmask_table[c] == 0b1000

                        bitmask_table[x not in {a, b, c}] == 0b0000
                        ⋃(bitmask_table[x] | x in {a, b, c}) == 0b1111
                 */
                bitmask_type operator [] (const key_type & element) const
                {
                    auto search_result = m_bitmasks.find(element);
                    if (search_result == m_bitmasks.end())
                    {
                        return 0b0;
                    }
                    else
                    {
                        return search_result->second;
                    }
                }

                //!     Длина исходной последовательности.
                std::size_t length () const
                {
                    return m_sequence_length;
                }

            private:
                map_type m_bitmasks;
                std::size_t m_sequence_length = 0;
            };

            //!     Позиционные маски элементов байтовой последовательности.
            /*!
                    Специализация для случая, когда входная последовательность состоит из
                однобайтовых чисел.
                    В этом случае можно в качестве отображения взять обычный массив и обращаться к
                нему просто по индексам.
             */
            template <typename Bitmask, std::size_t Size>
            class element_position_bitmask_table <std::array<Bitmask, Size>>
            {
            public:
                using array_type = std::array<Bitmask, Size>;
                using key_type = typename array_type::size_type;
                using bitmask_type = typename array_type::value_type;

            public:
                template <typename InputIterator>
                element_position_bitmask_table
                (
                    InputIterator sequence_begin,
                    InputIterator sequence_end
                ):
                    m_bitmasks{{0}}
                {
                    initialize(sequence_begin, sequence_end);
                }

                template <typename InputRange>
                explicit element_position_bitmask_table (const InputRange & sequence):
                    m_bitmasks{{0}}
                {
                    initialize(sequence.begin(), sequence.end());
                }

                explicit element_position_bitmask_table (std::initializer_list<key_type> sequence):
                    m_bitmasks{{0}}
                {
                    initialize(sequence.begin(), sequence.end());
                }

            private:
                template <typename InputIterator>
                void initialize (InputIterator first, InputIterator last)
                {
                    using iterated_type = iterator_value_t<InputIterator>;
                    static_assert
                    (
                        std::is_integral<iterated_type>::value && sizeof(iterated_type) == 1,
                        "Входной элемент должен быть однобайтовым целым числом."
                    );

                    auto elements_count = std::size_t{0};

                    auto position_indicator = bitmask_type{0b1};
                    while (first != last)
                    {
                        m_bitmasks[static_cast<unsigned char>(*first)] |= position_indicator;

                        ++first;
                        position_indicator = left_shift(position_indicator, 1u);
                        ++elements_count;
                    }

                    m_sequence_length = elements_count;
                }

            public:
                template <typename Integer>
                bitmask_type operator [] (Integer element) const
                {
                    static_assert(std::is_integral<Integer>::value && sizeof(Integer) == 1,
                        "Входной элемент должен быть однобайтовым целым числом.");

                    return m_bitmasks[static_cast<unsigned char>(element)];
                }

                //!     Длина исходной последовательности.
                std::size_t length () const
                {
                    return m_sequence_length;
                }

            private:
                array_type m_bitmasks;
                std::size_t m_sequence_length = 0;
            };
        } // namespace detail
    } // namespace algorithm
} // namespace burst

#endif // BURST__ALGORITHM__SEARCHING__DETAIL__ELEMENT_POSITION_BITMASK_TABLE_HPP
