#ifndef BURST_TEST_UTILITY_RANDOM_VECTOR_HPP
#define BURST_TEST_UTILITY_RANDOM_VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

namespace utility
{
    template <typename Integer>
    std::vector<Integer> random_vector (std::size_t size, Integer min, Integer max)
    {
        std::vector<Integer> numbers(size);

        std::uniform_int_distribution<Integer> uniform(min, max);
        std::default_random_engine generator;

        std::generate_n(numbers.begin(), numbers.size(),
            [& uniform, & generator]
            {
                return uniform(generator);
            });

        return numbers;
    }
}

#endif // BURST_TEST_UTILITY_RANDOM_VECTOR_HPP
