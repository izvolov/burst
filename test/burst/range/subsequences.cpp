#include <burst/range/subsequences.hpp>
#include <utility/io/vector.hpp>

#include <doctest/doctest.h>

#include <vector>

TEST_SUITE("subsequences")
{
    TEST_CASE("Элементы не обязаны быть упорядоченными")
    {
        const auto sequence = {3, 4, 1, -5};

        const auto subsequences = burst::subsequences(sequence);

        const auto expected_subsequences =
            std::vector<std::vector<int>>
            {
                {3}, {4}, {1}, {-5},
                {3, 4}, {3, 1}, {3, -5}, {4, 1}, {4, -5}, {1, -5},
                {3, 4, 1}, {3, 4, -5}, {3, 1, -5}, {4, 1, -5},
                {3, 4, 1, -5}
            };
        CHECK(subsequences == expected_subsequences);
    }
}
