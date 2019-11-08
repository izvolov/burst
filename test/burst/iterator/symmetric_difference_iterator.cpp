#include <burst/iterator/symmetric_difference_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("symmetric_difference_iterator")
{
    TEST_CASE("Конец итератора симметрической разности создаётся из его начала с помощью "
        "специальной метки-индикатора")
    {
        const auto  first = {   1, 2, 3   };
        const auto second = {0,    2,    4};
        const auto  third = {0,    2, 3   };
        //                      ^  ^     ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto symmetric_difference_begin =
            burst::make_symmetric_difference_iterator(ranges);
        auto symmetric_difference_end =
            burst::make_symmetric_difference_iterator(burst::iterator::end_tag, symmetric_difference_begin);

        const auto expected_collection = {1, 2, 4};
        const auto symmetric_difference =
            boost::make_iterator_range(symmetric_difference_begin, symmetric_difference_end);
        CHECK(symmetric_difference == expected_collection);
    }

    TEST_CASE("Симметрическая разность деструктивна по отношению к внутренним диапазонам")
    {
        const auto  first = {1, 2, 3, 4         };
        const auto second = {      3, 4, 5, 6   };
        const auto  third = {         4, 5, 6, 7};

        auto ranges = burst::make_range_vector(first, second, third);

        const auto symmetric_difference_begin =
            burst::make_symmetric_difference_iterator(ranges);
        const auto symmetric_difference_end =
            burst::make_symmetric_difference_iterator(burst::iterator::end_tag, symmetric_difference_begin);

        // Проход по всем элементам симметрической разности.
        const auto size = std::distance(symmetric_difference_begin, symmetric_difference_end);
        CHECK(size == 4);

        CHECK(ranges[0] != first);
        CHECK(ranges[1] != second);
        CHECK(ranges[2] != third);
    }
}
