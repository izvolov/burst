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

    TEST_CASE("Метод find_first возвращает пустой диапазон, если корпус короче образца")
    {
        const auto pattern = std::string("aab");
        const auto corpus = std::string("aa");

        using bitmask_type = std::uint16_t;
        const auto bitap = burst::algorithm::bitap<char, bitmask_type>(pattern);

        auto hint = bitmask_type{0};
        const auto match = bitap.find_first(corpus.begin(), corpus.end(), hint);

        CHECK(match.empty());
    }

    TEST_CASE("Метод find_first возвращает пустой диапазон, если образец не найден")
    {
        const auto pattern = std::string("aab");
        const auto corpus = std::string("aac");

        using bitmask_type = std::uint8_t;
        const auto bitap = burst::algorithm::bitap<char, bitmask_type>(pattern);

        auto hint = bitmask_type{0};
        const auto match = bitap.find_first(corpus.begin(), corpus.end(), hint);

        CHECK(match.empty());
    }

    TEST_CASE("Метод find_first возвращает диапазон итераторов на корпус, если образец найден")
    {
        const auto pattern = std::string(u8"аб");
        const auto corpus = std::string(pattern + pattern + pattern);

        using bitmask_type = std::bitset<32>;
        const auto bitap = burst::algorithm::bitap<char, bitmask_type>(pattern);

        auto hint = bitmask_type{0};
        const auto match = bitap.find_first(corpus.begin(), corpus.end(), hint);

        CHECK(match.begin() == corpus.begin());
        CHECK(match.end() == corpus.begin() + std::distance(pattern.begin(), pattern.end()));
        CHECK(match == pattern);
    }

    TEST_CASE("Если образец найден, то метод find_first изменяет подсказку таким образом, что "
        "в подсказке взведён бит на позиции, равной длине строки")
    {
        const auto pattern = std::string("aab");
        const auto corpus = std::string("aabbcc");

        using bitmask_type = std::uint32_t;
        using bitap_type = burst::algorithm::bitap<char, bitmask_type>;
        const auto bitap = bitap_type(pattern);

        auto hint = bitmask_type{0};
        bitap.find_first(pattern.begin(), pattern.end(), hint);

        CHECK(hint == bitmask_type{0b100});
    }

    TEST_CASE("Метод find_next возвращает диапазон итераторов на корпус, если образец найден")
    {
        const auto pattern = std::string(u8"аб");
        const auto corpus = std::string(pattern + pattern + pattern);
        const auto pattern_length = std::distance(pattern.begin(), pattern.end());

        using bitmask_type = std::bitset<32>;
        const auto bitap = burst::algorithm::bitap<char, bitmask_type>(pattern);
        auto hint = bitmask_type{0};
        const auto first_match = bitap.find_first(corpus.begin(), corpus.end(), hint);

        const auto second_match =
            bitap.find_next(first_match.begin(), first_match.end(), corpus.end(), hint);
        CHECK(second_match.begin() == corpus.begin() + pattern_length);
        CHECK(second_match.end() == corpus.begin() + pattern_length * 2);
        CHECK(second_match == pattern);

        const auto third_match =
            bitap.find_next(second_match.begin(), second_match.end(), corpus.end(), hint);
        CHECK(third_match.begin() == corpus.begin() + pattern_length * 2);
        CHECK(third_match.end() == corpus.begin() + pattern_length * 3);
        CHECK(third_match == pattern);
    }

    TEST_CASE("Метод find_next находит следующее вхождение образца с учётом наложения")
    {
        const auto pattern = std::string(u8"шалаш");
        const auto corpus = std::string(u8"шалашалаш");
        const auto pattern_length = std::distance(pattern.begin(), pattern.end());

        using bitmask_type = std::bitset<32>;
        const auto bitap = burst::algorithm::bitap<char, bitmask_type>(pattern);
        auto hint = bitmask_type{0};
        const auto first_match = bitap.find_first(corpus.begin(), corpus.end(), hint);

        const auto second_match =
            bitap.find_next(first_match.begin(), first_match.end(), corpus.end(), hint);
        CHECK(std::distance(corpus.begin(), second_match.begin()) < pattern_length);
        CHECK(second_match.begin() == corpus.end() - pattern_length);
        CHECK(second_match.end() == corpus.end());
        CHECK(second_match == pattern);
    }

    TEST_CASE("Метод find_next возвращает пустой диапазон, если образец не найден")
    {
        const auto pattern = std::wstring(L"Абырвалг");
        const auto corpus = pattern + L"што";

        using bitmask_type = std::bitset<32>;
        const auto bitap = burst::algorithm::bitap<wchar_t, bitmask_type>(pattern);
        auto hint = bitmask_type{0};
        const auto first_match = bitap.find_first(corpus.begin(), corpus.end(), hint);

        const auto second_match =
            bitap.find_next(first_match.begin(), first_match.end(), corpus.end(), hint);
        CHECK(second_match.empty());
    }

    TEST_CASE("Метод find_next возвращает пустой диапазон, если корпус закончился")
    {
        const auto pattern = std::wstring(L"Абырвалг");
        const auto corpus = pattern;

        using bitmask_type = std::bitset<32>;
        const auto bitap = burst::algorithm::bitap<wchar_t, bitmask_type>(pattern);
        auto hint = bitmask_type{0};
        const auto first_match = bitap.find_first(corpus.begin(), corpus.end(), hint);

        const auto second_match =
            bitap.find_next(first_match.begin(), first_match.end(), corpus.end(), hint);
        CHECK(second_match.empty());
    }

    TEST_CASE("Подсказка может быть использована для поиска в несвязных диапазонах")
    {
        const auto pattern = std::string("pattern");
        const auto cor = std::string("pat");
        const auto pus = std::string("tern");

        using bitmask_type = std::bitset<32>;
        const auto bitap = burst::algorithm::bitap<char, bitmask_type>(pattern);
        auto hint = bitmask_type{0};
        const auto no_match = bitap.find_first(cor.begin(), cor.end(), hint);
        static_cast<void>(no_match);

        const auto match = bitap.find_first(pus.begin(), pus.end(), hint);
        CHECK(hint == bitmask_type{1u << (pattern.size() - 1)});
        CHECK(not match.empty());
    }
}
