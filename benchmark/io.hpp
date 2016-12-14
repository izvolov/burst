#ifndef BURST_BENCHMARK_IO_HPP
#define BURST_BENCHMARK_IO_HPP

#include <burst/functional/trivial_read.hpp>
#include <burst/functional/trivial_write.hpp>
#include <burst/iterator/binary_ostream_iterator.hpp>

#include <boost/range/algorithm/transform.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>

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

template <typename Container>
std::istream & read (std::istream & stream, Container & values)
{
    auto size = std::size_t{};
    if (burst::trivial_read(stream, size))
    {
        values.resize(size);

        for (auto & value: values)
        {
            auto n = std::int64_t{};
            burst::trivial_read(stream, n);
            value = static_cast<typename Container::value_type>(n);
        }
    }

    return stream;
}

template <typename Container>
std::istream & read_many (std::istream & stream, Container & containers)
{
    auto container = typename Container::value_type{};
    while (read(stream, container))
    {
        containers.push_back(container);
    }

    return stream;
}

#endif // BURST_BENCHMARK_IO_HPP
