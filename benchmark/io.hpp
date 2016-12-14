#ifndef BURST_BENCHMARK_IO_HPP
#define BURST_BENCHMARK_IO_HPP

#include <boost/range/algorithm/for_each.hpp>

#include <iostream>
#include <sstream>
#include <string>

template <typename Container>
std::ostream & write (std::ostream & stream, const Container & values)
{
    boost::for_each(values, [& stream] (std::int64_t value) { stream << value << ' '; });
    return stream;
}

template <typename Container>
std::istream & read (std::istream & stream, Container & values)
{
    for (std::int64_t n; stream >> n; /* пусто */)
    {
        values.push_back(static_cast<typename Container::value_type>(n));
    }

    return stream;
}

template <typename Container>
std::istream & read_many (std::istream & stream, Container & containers)
{
    for (std::string line; std::getline(stream, line); /* пусто */)
    {
        containers.resize(containers.size() + 1);

        std::stringstream line_stream(line);
        read(line_stream, containers.back());
    }

    return stream;
}

#endif // BURST_BENCHMARK_IO_HPP
