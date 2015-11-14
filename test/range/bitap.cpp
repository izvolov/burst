#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iterator>
#include <string>
#include <unordered_map>

#include <boost/test/unit_test.hpp>

#include <burst/container/make_forward_list.hpp>
#include <burst/range/bitap.hpp>

BOOST_AUTO_TEST_SUITE(bitap)
    BOOST_AUTO_TEST_CASE(pattern_is_found_when_equal_to_whole_text)
    {
        const std::string pattern("Привет!");
        const std::string text(pattern);

        const auto matches = burst::bitap<std::uint16_t>(pattern, text);
        BOOST_REQUIRE(not matches.empty());

        const auto match = matches.front();
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            match.begin(), match.end(),
            text.begin(), text.end()
        );
    }

    BOOST_AUTO_TEST_CASE(pattern_is_found_at_the_first_match_in_a_text_having_several_matches)
    {
        const std::string pattern("абырвалг");
        const std::string text(pattern + "абырвалгович");

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        BOOST_REQUIRE(not matches.empty());

        const auto match = matches.front();
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            match.begin(), match.end(),
            text.begin(), text.begin() + static_cast<std::string::difference_type>(pattern.size())
        );
    }

    BOOST_AUTO_TEST_CASE(pattern_is_found_at_the_end_of_a_text)
    {
        const std::string pattern("ойойой");
        const std::string text("FFUUU" + pattern);

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        BOOST_REQUIRE(not matches.empty());

        const auto match = matches.front();

        typedef typename std::iterator_traits<std::string::iterator>::difference_type difference_type;
        const difference_type match_offset = static_cast<difference_type>(text.find_first_not_of("FU"));

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            match.begin(), match.end(),
            text.begin() + match_offset, text.end()
        );
    }

    BOOST_AUTO_TEST_CASE(pattern_is_not_found_in_a_text_with_no_matches)
    {
        const std::string pattern("123");
        const std::string text("12131415");

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);

        BOOST_CHECK(matches.empty());
    }

    BOOST_AUTO_TEST_CASE(all_overlapping_patterns_are_found)
    {
        const std::string pattern("шалаш");
        const std::string text("   шалашалаш   ");

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        BOOST_REQUIRE(not matches.empty());
        BOOST_REQUIRE_EQUAL(std::distance(matches.begin(), matches.end()), 2);

        std::for_each(matches.begin(), matches.end(),
            [& pattern] (const auto & match)
            {
                BOOST_CHECK_EQUAL_COLLECTIONS
                (
                    match.begin(), match.end(),
                    pattern.begin(), pattern.end()
                );
            });
    }

    BOOST_AUTO_TEST_CASE(matches_iterate_exactly_over_searched_text)
    {
        const std::string pattern("шалаш");
        const std::string text("   шалашалаш   ");

        auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        BOOST_REQUIRE(not matches.empty());
        BOOST_REQUIRE_EQUAL(std::distance(matches.begin(), matches.end()), 2);

        typedef typename std::iterator_traits<std::string::iterator>::difference_type difference_type;
        const auto first_match_start_position = static_cast<difference_type>(text.find_first_not_of(" "));
        const auto second_match_end_position = static_cast<difference_type>(text.find_first_of(" ", static_cast<std::size_t>(first_match_start_position)));
        const auto pattern_size = static_cast<difference_type>(pattern.size());

        const auto first_match = matches.front();
        BOOST_CHECK(first_match.begin() == text.begin() + first_match_start_position);
        BOOST_CHECK(first_match.end() == text.begin() + first_match_start_position + pattern_size);

        matches.advance_begin(1);
        const auto second_match = matches.front();
        BOOST_CHECK(second_match.begin() == text.begin() + (second_match_end_position - pattern_size));
        BOOST_CHECK(second_match.end() == text.begin() + second_match_end_position);
    }

    BOOST_AUTO_TEST_CASE(forward_iterator_is_enough_for_searching)
    {
        const auto pattern = burst::make_forward_list({1, 2, 3});
        const auto text = burst::make_forward_list({0, 1, 2, 3, 4});

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        BOOST_REQUIRE(not matches.empty());

        const auto match = matches.front();

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            match.begin(), match.end(),
            pattern.begin(), pattern.end()
        );
    }

    BOOST_AUTO_TEST_CASE(bitmask_map_can_be_set_explicitly)
    {
        const auto pattern = burst::make_forward_list({1, 2, 3});
        const auto text = burst::make_forward_list({0, 1, 2, 3, 4});

        using bitmask_type = std::bitset<32>;
        using bitmask_map_type = std::unordered_map<int, bitmask_type>;
        const auto matches = burst::bitap<bitmask_type, bitmask_map_type>(pattern, text);
        BOOST_REQUIRE(not matches.empty());

        const auto match = matches.front();

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            match.begin(), match.end(),
            pattern.begin(), pattern.end()
        );
    }
BOOST_AUTO_TEST_SUITE_END()
