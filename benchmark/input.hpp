#ifndef BURST_BENCHMARK_INPUT_HPP
#define BURST_BENCHMARK_INPUT_HPP

#include <iostream>

template <typename Container>
void read (std::istream & stream, Container & values)
{
    for (std::int64_t n; stream >> n; /* пусто */)
    {
        values.push_back(static_cast<typename Container::value_type>(n));
    }
}

#endif // BURST_BENCHMARK_INPUT_HPP
