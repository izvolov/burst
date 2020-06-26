#ifndef BURST_BENCHMARK_UTILITY_IO_WRITE_HPP
#define BURST_BENCHMARK_UTILITY_IO_WRITE_HPP

#include <burst/functional/trivial_write.hpp>
#include <burst/iterator/binary_ostream_iterator.hpp>

#include <boost/range/algorithm/transform.hpp>

#include <cstddef>
#include <cstdint>
#include <ostream>

namespace utility
{
    template <typename Container>
    std::ostream & write (std::ostream & stream, const Container & values)
    {
        burst::trivial_write(stream, static_cast<std::size_t>(values.size()));
        boost::transform(values, burst::make_binary_ostream_iterator(stream),
            [] (std::int64_t v) {return v;});
        // Элементы контейнера должны уметь неявно (то есть без потерь) преобразовываться к
        // std::int64_t, т.к. это "официальный" формат обмена числами между приложениями.

        return stream;
    }
} // namespace utility

#endif // BURST_BENCHMARK_UTILITY_IO_WRITE_HPP
