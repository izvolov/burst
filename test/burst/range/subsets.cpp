#include <burst/range/subsets.hpp>

#include <doctest/doctest.h>

#include <functional>
#include <set>

TEST_SUITE("subsets")
{
    TEST_CASE("repeating_elements_are_not_considered")
    {
        auto sequence = {3, 3, 3, 2, 2, 2, 1, 1, 1};
        std::set<int, std::greater<>> set(sequence.begin(), sequence.end());

        auto sequence_subsets = burst::subsets(sequence, std::greater<>{});
        auto set_subsets = burst::subsets(set, set.key_comp());

        CHECK(sequence_subsets == set_subsets);
    }
}
