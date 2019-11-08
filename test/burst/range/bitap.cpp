#include <burst/container/make_forward_list.hpp>
#include <burst/range/bitap.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iterator>
#include <string>
#include <unordered_map>

TEST_SUITE("bitap")
{
    TEST_CASE("Находит в тесте образец, равный всему тексту")
    {
        const std::string pattern("Привет!");
        const std::string text(pattern);

        const auto matches = burst::bitap<std::uint16_t>(pattern, text);
        REQUIRE(not matches.empty());

        const auto match = matches.front();
        CHECK(match == text);
    }

    TEST_CASE("Возвращает первое вхождение образца в случае, если в тексте их несколько")
    {
        const std::string pattern("абырвалг");
        const std::string text(pattern + "абырвалгович");

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        REQUIRE(not matches.empty());

        const auto match = matches.front();
        CHECK(match == text.substr(0, pattern.size()));
    }

    TEST_CASE("Находит образец, если текст заканчивается им")
    {
        const std::string pattern("ойойой");
        const std::string text("FFUUU" + pattern);

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        REQUIRE(not matches.empty());

        const auto match = matches.front();

        const auto match_offset = text.find_first_not_of("FU");
        CHECK(match == text.substr(match_offset));
    }

    TEST_CASE("Возвращает пустой диапазон, если в тексте отсутствуют совпадения")
    {
        const std::string pattern("123");
        const std::string text("12131415");

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);

        CHECK(matches.empty());
    }

    TEST_CASE("Находит все вхождения образцов, даже если они накладываются друг на друга в тексте")
    {
        const std::string pattern("шалаш");
        const std::string text("   шалашалаш   ");

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        REQUIRE(not matches.empty());
        REQUIRE(std::distance(matches.begin(), matches.end()) == 2);

        std::for_each(matches.begin(), matches.end(),
            [& pattern] (const auto & match)
            {
                CHECK(match == pattern);
            });
    }

    TEST_CASE("Каждое совпадение является диапазоном итераторы которого задают позиции в "
        "исходном тексте-диапазоне")
    {
        const std::string pattern("шалаш");
        const std::string text("   шалашалаш   ");

        auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        REQUIRE(not matches.empty());
        REQUIRE(std::distance(matches.begin(), matches.end()) == 2);

        using difference_type = typename std::iterator_traits<std::string::iterator>::difference_type;
        const auto first_match_start_position = static_cast<difference_type>(text.find_first_not_of(" "));
        const auto second_match_end_position = static_cast<difference_type>(text.find_first_of(" ", static_cast<std::size_t>(first_match_start_position)));
        const auto pattern_size = static_cast<difference_type>(pattern.size());

        const auto first_match = matches.front();
        CHECK(first_match.begin() == text.begin() + first_match_start_position);
        CHECK(first_match.end() == text.begin() + first_match_start_position + pattern_size);

        matches.advance_begin(1);
        const auto second_match = matches.front();
        CHECK(second_match.begin() == text.begin() + (second_match_end_position - pattern_size));
        CHECK(second_match.end() == text.begin() + second_match_end_position);
    }

    TEST_CASE("Для поиска достаточно однонаправленного итератора")
    {
        const auto pattern = burst::make_forward_list({1, 2, 3});
        const auto text = burst::make_forward_list({0, 1, 2, 3, 4});

        const auto matches = burst::bitap<std::bitset<32>>(pattern, text);
        REQUIRE(not matches.empty());

        const auto match = matches.front();

        CHECK(match == pattern);
    }

    TEST_CASE("Допускает пользовательское отображение элементов в битовые маски")
    {
        const auto pattern = burst::make_forward_list({1, 2, 3});
        const auto text = burst::make_forward_list({0, 1, 2, 3, 4});

        using bitmask_type = std::bitset<32>;
        using bitmask_map_type = std::unordered_map<int, bitmask_type>;
        const auto matches = burst::bitap<bitmask_type, bitmask_map_type>(pattern, text);
        REQUIRE(not matches.empty());

        const auto match = matches.front();

        CHECK(match == pattern);
    }
}
