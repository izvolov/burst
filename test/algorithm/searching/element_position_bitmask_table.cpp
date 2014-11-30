#include <map>
#include <set>
#include <string>
#include <bitset>

#include <boost/test/unit_test.hpp>
#include <boost/utility/binary.hpp>

#include <thrust/algorithm/searching/detail/element_position_bitmask_table.hpp>

typedef thrust::algorithm::detail::element_position_bitmask_table<std::map<char, std::bitset<32>>> char_masks;
typedef thrust::algorithm::detail::element_position_bitmask_table<std::map<int, std::bitset<64>>> integral_masks;

BOOST_AUTO_TEST_SUITE(ElementPositionBitmaskTable)

BOOST_AUTO_TEST_CASE(mask_created_from_empty_pattern_is_empty)
{
    char_masks pattern_masks(std::string(""));
    BOOST_CHECK_EQUAL(pattern_masks.length(), 0ul);
}

BOOST_AUTO_TEST_CASE(one_element_pattern)
{
    char_masks pattern_masks(std::string("a"));
    BOOST_CHECK_EQUAL(pattern_masks['a'], BOOST_BINARY(1));
}

BOOST_AUTO_TEST_CASE(pattern_with_repeating_elements)
{
    integral_masks pattern_masks{1, 2, 1, 3};
    BOOST_CHECK_EQUAL(pattern_masks[1], BOOST_BINARY(0101));
    BOOST_CHECK_EQUAL(pattern_masks[2], BOOST_BINARY(0010));
    BOOST_CHECK_EQUAL(pattern_masks[3], BOOST_BINARY(1000));
}

BOOST_AUTO_TEST_CASE(bitmask_for_nonexistent_element_is_zero)
{
    char_masks pattern_masks(std::string("qwertyqwerty"));

    BOOST_CHECK_EQUAL(pattern_masks['a'], BOOST_BINARY(0));
    BOOST_CHECK_EQUAL(pattern_masks['b'], BOOST_BINARY(0));
}

BOOST_AUTO_TEST_CASE(disjunction_of_masks_for_every_pattern_element_has_all_bits_set_to_1)
{
    std::string pattern("abac");
    char_masks pattern_masks(pattern);

    std::set<char> unique_pattern_elements(pattern.begin(), pattern.end());

    char_masks::bitmask_type disjunction = 0;
    for (char element: unique_pattern_elements)
    {
        disjunction |= pattern_masks[element];
    }

    BOOST_CHECK_EQUAL(disjunction, (0x01 << pattern.length()) - 1);
}

BOOST_AUTO_TEST_SUITE_END()
