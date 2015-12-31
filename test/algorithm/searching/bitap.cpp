#include <bitset>
#include <string>
#include <forward_list>

#include <boost/test/unit_test.hpp>

#include <burst/algorithm/searching/bitap.hpp>

BOOST_AUTO_TEST_SUITE(bitap)
    BOOST_AUTO_TEST_CASE(pattern_is_found_at_the_next_match_when_search_started_in_the_middle_of_the_first_match)
    {
        std::string pattern("asdf");
        std::string text(pattern + pattern);
        burst::algorithm::bitap<char, std::bitset<32>> search(pattern);

        auto match_position = search(text.begin() + 1, text.end());

        using difference_type = typename std::iterator_traits<std::string::iterator>::difference_type;
        difference_type repeat_offset = static_cast<difference_type>(pattern.size());
        BOOST_CHECK(match_position == text.begin() + repeat_offset);
    }
BOOST_AUTO_TEST_SUITE_END()
