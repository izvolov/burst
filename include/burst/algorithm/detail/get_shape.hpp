#ifndef BURST__ALGORITHM__DETAIL__GET_SHAPE_HPP
#define BURST__ALGORITHM__DETAIL__GET_SHAPE_HPP

#include <burst/execution/parallel_policy.hpp>
#include <burst/integer/divceil.hpp>

#include <cstddef>
#include <array>

namespace burst
{
    namespace detail
    {
        /*!
            \brief
                Определить форму для параллельного исполнения

            \returns
                Массив из двух значений, где `array[0]` — количество потоков, `array[1]` — размер
                куска для одного потока (размер последнего куска по факту может оказаться меньше
                полного куска, эту ситуацию нужно обрабатывать по месту использования).
                Если диапазон пуст, то возвращается {0, 0}.
         */
        template <typename RandomAccessIterator>
        auto get_shape (parallel_policy par, RandomAccessIterator first, RandomAccessIterator last)
        {
            using std::distance;
            if (first != last)
            {
                const auto range_size = static_cast<std::size_t>(distance(first, last));
                const auto chunk_size = divceil(range_size, par.thread_count);
                const auto thread_count = divceil(range_size, chunk_size);

                return std::array<std::size_t, 2>{thread_count, chunk_size};
            }
            else
            {
                return std::array<std::size_t, 2>{0, 0};
            }
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__GET_SHAPE_HPP
