#include <burst/algorithm/searching/detail/element_position_bitmask_table.hpp>

#include <doctest/doctest.h>

#include <boost/range/numeric.hpp>

#include <array>
#include <bitset>
#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <string>

using char_masks = burst::algorithm::detail::element_position_bitmask_table<std::array<std::bitset<32>, (1 << sizeof(char) * CHAR_BIT)>>;
using integral_masks = burst::algorithm::detail::element_position_bitmask_table<std::map<int, std::bitset<64>>>;

TEST_SUITE("element_position_bitmask_table")
{
    TEST_CASE("length_of_bitmask_table_created_from_empty_pattern_is_zero")
    {
        const auto pattern_masks = char_masks(std::string(""));
        CHECK(pattern_masks.length() == 0ul);
    }

    TEST_CASE("length_of_bitmask_table_created_from_non_empty_pattern_is_equal_to_length_of_the_pattern")
    {
        const auto pattern_masks = char_masks(std::string("abcabc"));
        CHECK(pattern_masks.length() == 6);
    }

    TEST_CASE("bits_in_all_positions_of_an_element_are_set_up")
    {
        const auto pattern_masks = integral_masks{1, 2, 1, 3};
        CHECK(pattern_masks[1] == 0b0101);
        CHECK(pattern_masks[2] == 0b0010);
        CHECK(pattern_masks[3] == 0b1000);
    }

    TEST_CASE("all_bits_higher_than_length_of_the_pattern_are_not_set")
    {
        const auto pattern = std::string("abc");
        const auto pattern_masks = char_masks(pattern);
        const auto unique_pattern_elements = std::set<char>(pattern.begin(), pattern.end());
        const auto all_ones = ~char_masks::bitmask_type{0b0};

        for (auto element: unique_pattern_elements)
        {
            const auto mask = pattern_masks[element];
            CHECK((mask & all_ones) == mask);
        }
    }

    TEST_CASE("bitmask_for_nonexistent_element_is_zero")
    {
        const auto pattern_masks = char_masks(std::string("qwertyqwerty"));

        CHECK(pattern_masks['a'] == 0b0);
        CHECK(pattern_masks['b'] == 0b0);
    }

    TEST_CASE("disjunction_of_masks_for_every_pattern_element_has_all_pattern_bits_set_to_1")
    {
        const auto pattern = std::string("abac");
        const auto pattern_masks = char_masks(pattern);

        const auto unique_pattern_elements = std::set<char>(pattern.begin(), pattern.end());
        const auto disjunction =
            boost::accumulate(unique_pattern_elements, char_masks::bitmask_type{0b0},
                [& pattern_masks] (auto mask, auto element)
                {
                    return mask | pattern_masks[element];
                });

        CHECK(disjunction == (0b1 << pattern.length()) - 1);
    }
}
