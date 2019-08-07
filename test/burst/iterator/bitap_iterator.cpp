#include <burst/algorithm/searching/bitap.hpp>
#include <burst/iterator/bitap_iterator.hpp>
#include <burst/iterator/end_tag.hpp>

#include <doctest/doctest.h>

#include <string>

TEST_SUITE("bitap_iterator")
{
    TEST_CASE("copied_iterator_is_independent")
    {
        const std::string pattern("два");
        const std::string text("дваждыдвачетыре");

        const auto bitap = burst::algorithm::make_bitap<std::uint32_t>(pattern);

        auto original = burst::make_bitap_iterator(bitap, text);
        const auto end = burst::make_bitap_iterator(burst::iterator::end_tag, original);

        auto copy = original;

        std::advance(original, 2);
        CHECK(original == end);

        CHECK(*copy == pattern);

        ++copy;
        CHECK(*copy == pattern);

        ++copy;
        CHECK(copy == end);
    }
}
