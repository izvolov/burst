#ifndef BURST_BENCHMARK_INPUT_HPP
#define BURST_BENCHMARK_INPUT_HPP

#include <iostream>
#include <sstream>
#include <string>

template <typename Container>
void read (std::istream & stream, Container & values)
{
    for (std::int64_t n; stream >> n; /* пусто */)
    {
        values.push_back(static_cast<typename Container::value_type>(n));
    }
}

template <typename Container>
void read_many (std::istream & stream, Container & containers)
{
    for (std::string line; std::getline(stream, line); /* пусто */)
    {
        containers.resize(containers.size() + 1);

        std::stringstream line_stream(line);
        read(line_stream, containers.back());
    }
}

#endif // BURST_BENCHMARK_INPUT_HPP
