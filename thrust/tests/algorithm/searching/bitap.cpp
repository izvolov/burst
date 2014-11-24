#include <bitset>
#include <string>
#include <unordered_map>
#include <forward_list>

#include <boost/test/unit_test.hpp>

#include <thrust/algorithm/searching/bitap.hpp>

BOOST_AUTO_TEST_SUITE(Bitap)

BOOST_AUTO_TEST_CASE(pattern_is_found_when_equal_to_whole_text)
{
    std::string pattern("Привет!");
    std::string text(pattern);
    thrust::algorithm::bitap<char, std::uint16_t> search(pattern);

    auto match_position = search(text.begin(), text.end());

    BOOST_CHECK(match_position == text.begin());
}

BOOST_AUTO_TEST_CASE(pattern_is_found_at_the_first_match_in_a_text_having_several_matches)
{
    std::string pattern("абырвалг");
    std::string text(pattern + "абырвалгович");
    thrust::algorithm::bitap<char, std::bitset<32>> search(pattern);

    auto match_position = search(text.begin(), text.end());

    BOOST_CHECK(match_position == text.begin());
}

BOOST_AUTO_TEST_CASE(pattern_is_found_at_the_end_of_a_text)
{
    std::string pattern("ойойой");
    std::string text("FFUUU" + pattern);
    thrust::algorithm::bitap<char, std::bitset<32>> search(pattern);

    auto match_position = search(text.begin(), text.end());

    typedef typename std::iterator_traits<std::string::iterator>::difference_type difference_type;
    difference_type match_offset = static_cast<difference_type>(text.find_first_not_of("FU"));
    BOOST_CHECK(match_position == text.begin() + match_offset);
}

BOOST_AUTO_TEST_CASE(pattern_is_not_found_in_a_text_with_no_matches)
{
    std::string pattern("123");
    std::string text("12131415");
    thrust::algorithm::bitap<char, std::bitset<32>> search(pattern);

    auto match_position = search(text.begin(), text.end());

    BOOST_CHECK(match_position == text.end());
}

BOOST_AUTO_TEST_CASE(pattern_is_found_at_the_next_match_when_search_started_in_the_middle_of_the_first_match)
{
    std::string pattern("asdf");
    std::string text(pattern + pattern);
    thrust::algorithm::bitap<char, std::bitset<32>> search(pattern);

    auto match_position = search(text.begin() + 1, text.end());

    typedef typename std::iterator_traits<std::string::iterator>::difference_type difference_type;
    difference_type repeat_offset = static_cast<difference_type>(pattern.size());
    BOOST_CHECK(match_position == text.begin() + repeat_offset);
}

BOOST_AUTO_TEST_CASE(first_of_two_overlapping_patterns_is_found)
{
    std::string pattern("шалаш");
    std::string text("   шалашалаш   ");
    thrust::algorithm::bitap<char, std::bitset<32>, std::unordered_map<char, std::bitset<32>>> search(pattern);

    auto match_position = search(text.begin(), text.end());

    typedef typename std::iterator_traits<std::string::iterator>::difference_type difference_type;
    difference_type non_space_offset = static_cast<difference_type>(text.find_first_not_of(" "));
    BOOST_CHECK(match_position == text.begin() + non_space_offset);
}

BOOST_AUTO_TEST_CASE(forward_iterator_is_enough_for_searching)
{
    std::forward_list<int> pattern{1, 2, 3};
    std::forward_list<int> text{0, 1, 2, 3, 4};
    thrust::algorithm::bitap<int, std::bitset<32>, std::unordered_map<int, std::bitset<32>>> search(pattern);

    auto match_position = search(text.begin(), text.end());

    BOOST_CHECK(match_position == std::find(text.begin(), text.end(), 1));
}

BOOST_AUTO_TEST_SUITE_END()
