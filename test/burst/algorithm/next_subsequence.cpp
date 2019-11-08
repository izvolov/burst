#include <burst/algorithm/next_subsequence.hpp>

#include <doctest/doctest.h>

#include <vector>

TEST_SUITE("next_subsequence")
{
    TEST_CASE("Принимает итераторы корректной подпоследовательности и итераторы исходной "
        "последовательности")
    {
        using vector_type = std::vector<int>;
        const auto items = vector_type{17};
        std::vector<vector_type::const_iterator> subsequence_container(items.size());

        auto new_subsequence_end =
            burst::next_subsequence
            (
                subsequence_container.begin(),
                subsequence_container.begin(),
                items.begin(),
                items.end()
            );

        CHECK(new_subsequence_end == subsequence_container.begin() + 1);
        CHECK(**subsequence_container.begin() == 17);
    }

    TEST_CASE("У пустой последовательности нет подпоследовательностей")
    {
        using vector_type = std::vector<int>;
        auto items = vector_type{};
        std::vector<vector_type::iterator> subsequence_container(10);

        auto new_subsequence_end =
            burst::next_subsequence
            (
                subsequence_container.begin(),
                subsequence_container.begin(),
                items.begin(),
                items.end()
            );

        CHECK(new_subsequence_end == subsequence_container.begin());
    }
}
