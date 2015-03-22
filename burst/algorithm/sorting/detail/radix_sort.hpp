#ifndef BURST_ALGORITHM_SORTING_DETAIL_RADIX_SORT_HPP
#define BURST_ALGORITHM_SORTING_DETAIL_RADIX_SORT_HPP

#include <algorithm>
#include <climits>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>

#include <burst/algorithm/sorting/counting_sort_copy.hpp>
#include <burst/variadic.hpp>

namespace burst
{
    namespace detail
    {
        //!     Целая часть двоичного логарифма.
        inline constexpr std::size_t log2ip (std::size_t integer)
        {
            // Из-за C++11 приходится использовать рекурсию.
            return integer > 0 ? log2ip(integer >> 1) + 1 : std::numeric_limits<std::size_t>::max();

            // А в C++14 можно обойтись без рекурсии.
            // std::size_t degree = std::numeric_limits<std::size_t>::max();

            // while (integer > 0)
            // {
            //     integer >>= 1;
            //     ++degree;
            // }

            // return degree;
        }

        template <typename Value, typename Map, typename Radix>
        struct radix_sort_traits
        {
            using integer_type = typename std::decay<typename std::result_of<Map(Value)>::type>::type;
            static_assert
            (
                std::is_integral<integer_type>::value && std::is_unsigned<integer_type>::value,
                "Сортируемые элементы должны быть отображены в целые беззнаковые числа."
            );

            using radix_type = typename std::decay<typename std::result_of<Radix(integer_type)>::type>::type;
            static_assert
            (
                std::is_integral<radix_type>::value,
                "Тип разряда, выделяемого из целого числа, тоже должен быть целым."
            );

            constexpr static const auto min_radix_value = std::numeric_limits<radix_type>::min();
            constexpr static const auto max_radix_value = std::numeric_limits<radix_type>::max();
            constexpr static const auto radix_value_range = max_radix_value - min_radix_value + 1;
            constexpr static const auto radix_size = detail::log2ip(radix_value_range);
            constexpr static const auto radix_count = sizeof(integer_type) * CHAR_BIT / radix_size;
        };

        template <typename Map, typename Radix>
        auto nth_radix (std::size_t radix_number, Map map, Radix radix)
        {
            return [radix_number, map = std::move(map), radix = std::move(radix)] (const auto & value)
            {
                using value_type = typename std::remove_reference<decltype(value)>::type;
                using traits = radix_sort_traits<value_type, Map, Radix>;

                return radix(static_cast<typename traits::integer_type>(map(value) >> (traits::radix_size * radix_number)));
            };
        }

        template <typename ForwardIterator, typename Map, typename Radix, typename Array, std::size_t ... Radices>
        void collect_impl (ForwardIterator first, ForwardIterator last, Map map, Radix radix, Array & counters, std::index_sequence<Radices...>)
        {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            using traits = detail::radix_sort_traits<value_type, Map, Radix>;

            std::for_each(first, last,
                [& counters, & map, & radix] (const value_type & value)
                {
                    BURST_VARIADIC(++counters[Radices][radix(static_cast<typename traits::integer_type>(map(value) >> (traits::radix_size * Radices))) - traits::min_radix_value + 1]);
                });

            BURST_VARIADIC(std::partial_sum(std::begin(counters[Radices]), std::end(counters[Radices]), std::begin(counters[Radices])));
        }

        //!     Собрать счётчики сразу для всех разрядов.
        /*!
                Для каждого сортируемого числа подсчитывает количество элементов, строго меньших
            этого числа.
         */
        template <typename ForwardIterator, typename Map, typename Radix, typename Array>
        void collect (ForwardIterator first, ForwardIterator last, Map map, Radix radix, Array & counters)
        {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            constexpr auto radix_count = detail::radix_sort_traits<value_type, Map, Radix>::radix_count;
            collect_impl(first, last, map, radix, counters, std::make_index_sequence<radix_count>());
        }

        //!     Сортировка с пользовательским буфером.
        /*!
                Один буфер задаётся пользователем, а второй — создаётся внутри функции.
                Сортировка по первому разряду проходит из входного диапазона во внутренний буфер.
            Затем сортировка проходит из одного буфера в другой до тех пор, пока не заканчиваются
            разряды. Итоговый результат сортировки оказывается в пользовательском буфере.
                Входной диапазон не изменяется.
         */
        template <typename ForwardIterator, typename RandomAccessIterator, typename Map, typename Radix>
        void radix_sort_with_custom_buffer (ForwardIterator first, ForwardIterator last, RandomAccessIterator buffer, Map map, Radix radix)
        {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            using traits = detail::radix_sort_traits<value_type, Map, Radix>;

            using difference_type = typename std::iterator_traits<ForwardIterator>::difference_type;
            difference_type counters[traits::radix_count][traits::radix_value_range + 1] = {{0}};
            detail::collect(first, last, map, radix, counters);

            auto distance = std::distance(first, last);
            std::vector<value_type> extra_buffer(static_cast<std::size_t>(distance));

            auto get_low_radix = [& radix, & map] (const value_type & value) { return radix(map(value)); };
            detail::dispose(first, last, extra_buffer.begin(), get_low_radix, counters[0]);

            for (std::size_t radix_number = 1; radix_number < traits::radix_count - 1; radix_number += 2)
            {
                detail::dispose
                (
                    extra_buffer.begin(),
                    extra_buffer.end(),
                    buffer,
                    nth_radix(radix_number, map, radix),
                    counters[radix_number]
                );
                detail::dispose
                (
                    buffer,
                    buffer + static_cast<typename std::iterator_traits<RandomAccessIterator>::difference_type>(distance),
                    extra_buffer.begin(),
                    nth_radix(radix_number + 1, map, radix),
                    counters[radix_number + 1]
                );
            }

            auto get_high_radix = nth_radix(traits::radix_count - 1, map, radix);
            detail::dispose(extra_buffer.begin(), extra_buffer.end(), buffer, get_high_radix, counters[traits::radix_count - 1]);
        }

        //!     Сортировка с двумя дополнительными буферами.
        /*!
                Внутри функции заводятся два дополнительных буфера. Сортировка по первому разряду
            проходит из входного диапазона в первый буфер. Дальнейшая сортировка происходит между
            двумя буферами. После того, как сортировка окончена, результат перемещается из буфера в
            выходной диапазон.
                Входной диапазон не изменяется.
         */
        template <typename ForwardIterator1, typename ForwardIterator2, typename Map, typename Radix>
        void radix_sort_with_two_buffers (ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 result, Map map, Radix radix)
        {
            using value_type = typename std::iterator_traits<ForwardIterator1>::value_type;
            using traits = detail::radix_sort_traits<value_type, Map, Radix>;

            using difference_type = typename std::iterator_traits<ForwardIterator1>::difference_type;
            difference_type counters[traits::radix_count][traits::radix_value_range + 1] = {{0}};
            detail::collect(first, last, map, radix, counters);

            auto distance = std::distance(first, last);
            std::vector<value_type> resulting_buffer(static_cast<std::size_t>(distance));
            std::vector<value_type> intermediate_buffer(static_cast<std::size_t>(distance));

            auto get_low_radix = [& radix, & map] (const value_type & value) { return radix(map(value)); };
            detail::dispose(first, last, resulting_buffer.begin(), get_low_radix, counters[0]);

            for (std::size_t radix_number = 1; radix_number < traits::radix_count; ++radix_number)
            {
                detail::dispose(resulting_buffer.begin(), resulting_buffer.end(),
                    intermediate_buffer.begin(),
                    nth_radix(radix_number, map, radix),
                    counters[radix_number]);

                std::swap(resulting_buffer, intermediate_buffer);
            }

            std::move(resulting_buffer.begin(), resulting_buffer.end(), result);
        }

        //!     Специализация для случая, когда в сортируемом числе всего один разряд.
        /*!
                Отличается от основной функции тем, что здесь заводится один дополнительный буфер,
            а не два.
         */
        template <typename ForwardIterator, typename Map, typename Radix>
        typename std::enable_if
        <
            detail::radix_sort_traits
            <
                typename std::iterator_traits<ForwardIterator>::value_type,
                Map,
                Radix
            >
            ::radix_count == 1,
            void
        >
        ::type radix_sort_impl (ForwardIterator first, ForwardIterator last, Map map, Radix radix)
        {
            auto distance = std::distance(first, last);
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            std::vector<value_type> buffer(static_cast<std::size_t>(distance));

            counting_sort_copy(first, last, buffer.begin(),
                [& map, & radix] (const value_type & value)
                {
                    return radix(map(value));
                });
            std::move(buffer.begin(), buffer.end(), first);
        }

        //!     Общий случай.
        /*!
                Вызывается тогда, когда разрядов в сортируемых числах больше одного.
         */
        template <typename ForwardIterator, typename Map, typename Radix>
        typename std::enable_if
        <
            (detail::radix_sort_traits
            <
                typename std::iterator_traits<ForwardIterator>::value_type,
                Map,
                Radix
            >
            ::radix_count > 1),
            void
        >
        ::type radix_sort_impl (ForwardIterator first, ForwardIterator last, Map map, Radix radix)
        {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            using traits = detail::radix_sort_traits<value_type, Map, Radix>;

            using difference_type = typename std::iterator_traits<ForwardIterator>::difference_type;
            difference_type counters[traits::radix_count][traits::radix_value_range + 1] = {{0}};
            detail::collect(first, last, map, radix, counters);

            auto distance = std::distance(first, last);
            std::vector<value_type> resulting_buffer(static_cast<std::size_t>(distance));
            std::vector<value_type> intermediate_buffer(static_cast<std::size_t>(distance));

            auto get_low_radix = [& radix, & map] (const value_type & value) { return radix(map(value)); };
            detail::dispose(first, last, resulting_buffer.begin(), get_low_radix, counters[0]);

            for (std::size_t radix_number = 1; radix_number < traits::radix_count; ++radix_number)
            {
                auto get_radix =
                    [& map, & radix, & radix_number] (const value_type & value)
                    {
                        return radix(static_cast<typename traits::integer_type>(map(value) >> (traits::radix_size * radix_number)));
                    };
                detail::dispose(resulting_buffer.begin(), resulting_buffer.end(),
                    intermediate_buffer.begin(),
                    get_radix,
                    counters[radix_number]);

                std::swap(resulting_buffer, intermediate_buffer);
            }

            std::move(resulting_buffer.begin(), resulting_buffer.end(), first);
        }

        //!     Специализация для случая, когда сортируется один разряд.
        /*!
                Просто вызывает сортировку подсчётом.
         */
        template <typename ForwardIterator, typename Iterator, typename Map, typename Radix>
        typename std::enable_if
        <
            detail::radix_sort_traits
            <
                typename std::iterator_traits<ForwardIterator>::value_type,
                Map,
                Radix
            >
            ::radix_count == 1,
            void
        >
        ::type radix_sort_copy_impl (ForwardIterator first, ForwardIterator last, Iterator result, Map map, Radix radix)
        {
            return counting_sort_copy_impl(first, last, result,
                [& map, & radix] (const auto & value)
                {
                    return radix(map(value));
                });
        }

        template <typename ForwardIterator, typename RandomAccessIterator, typename Map, typename Radix>
        typename std::enable_if
        <
            std::is_same
            <
                typename std::iterator_traits<RandomAccessIterator>::iterator_category,
                std::random_access_iterator_tag
            >
            ::value && detail::radix_sort_traits
            <
                typename std::iterator_traits<ForwardIterator>::value_type,
                Map,
                Radix
            >
            ::radix_count % 2 == 0,
            void
        >
        ::type radix_sort_copy_impl (ForwardIterator first, ForwardIterator last, RandomAccessIterator result, Map map, Radix radix)
        {
            return radix_sort_with_custom_buffer(first, last, result, map, radix);
        }

        template <typename ForwardIterator1, typename ForwardIterator2, typename Map, typename Radix>
        typename std::enable_if
        <
            not std::is_same
            <
                typename std::iterator_traits<ForwardIterator2>::iterator_category,
                std::random_access_iterator_tag
            >
            ::value && detail::radix_sort_traits
            <
                typename std::iterator_traits<ForwardIterator1>::value_type,
                Map,
                Radix
            >
            ::radix_count % 2 == 0,
            void
        >
        ::type radix_sort_copy_impl (ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 result, Map map, Radix radix)
        {
            return radix_sort_with_two_buffers(first, last, result, map, radix);
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_DETAIL_RADIX_SORT_HPP
