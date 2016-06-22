#include <burst/algorithm/searching/bitap.hpp>
#include <burst/iterator/bitap_iterator.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/test/unit_test.hpp>

#include <string>

BOOST_AUTO_TEST_SUITE(bitap_iterator)
    BOOST_AUTO_TEST_CASE(copied_iterator_is_independent)
    {
        const std::string pattern("два");
        const std::string text("дваждыдвачетыре");

        const auto bitap = burst::algorithm::make_bitap<std::uint32_t>(pattern);

        auto original = burst::make_bitap_iterator(bitap, text);
        const auto end = burst::make_bitap_iterator(original, burst::iterator::end_tag);

        auto copy = original;

        std::advance(original, 2);
        BOOST_CHECK(original == end);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            copy->begin(), copy->end(),
            pattern.begin(), pattern.end()
        );

        ++copy;
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            copy->begin(), copy->end(),
            pattern.begin(), pattern.end()
        );

        ++copy;
        BOOST_CHECK(copy == end);
    }
BOOST_AUTO_TEST_SUITE_END()
