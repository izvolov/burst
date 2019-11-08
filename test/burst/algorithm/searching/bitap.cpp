#include <burst/algorithm/searching/bitap.hpp>

#include <doctest/doctest.h>

#include <bitset>
#include <forward_list>
#include <string>

TEST_SUITE("bitap")
{
    TEST_CASE("Если поиск начинается где-то в середине совпадения, то образец будет найден в "
        "следующем совпадении")
    {
        std::string pattern("asdf");
        std::string text(pattern + pattern);
        burst::algorithm::bitap<char, std::bitset<32>> search(pattern);

        auto match_position = search(text.begin() + 1, text.end());

        using difference_type = typename std::iterator_traits<std::string::iterator>::difference_type;
        difference_type repeat_offset = static_cast<difference_type>(pattern.size());
        CHECK(match_position == text.begin() + repeat_offset);
    }
}
