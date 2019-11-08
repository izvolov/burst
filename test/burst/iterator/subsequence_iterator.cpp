#include <burst/integer/intpow.hpp>
#include <burst/iterator/subsequence_iterator.hpp>
#include <utility/io/vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <iterator>
#include <list>
#include <vector>

TEST_SUITE("subsequence_iterator")
{
    TEST_CASE("Конец итератора подпоследовательностей создаётся из его начала с помощью "
        "специальной метки-индикатора")
    {
        const auto sequence = {'a', 'b', 'c'};

        const auto subsequences_begin = burst::make_subsequence_iterator(sequence);
        const auto subsequences_end =
            burst::make_subsequence_iterator(burst::iterator::end_tag, subsequences_begin);

        const auto expected_subsequences =
            std::vector<std::vector<char>>
            {
                {'a'}, {'b'}, {'c'},
                {'a', 'b'}, {'a', 'c'}, {'b', 'c'},
                {'a', 'b', 'c'}
            };
        const auto subsequences = boost::make_iterator_range(subsequences_begin, subsequences_end);
        CHECK(subsequences == expected_subsequences);
    }

    TEST_CASE("Пустая подпоследовательность не содержит подпоследовательностей")
    {
        const auto sequence = std::vector<int>{};

        const auto subsequences_begin = burst::make_subsequence_iterator(sequence);
        const auto subsequences_end =
            burst::make_subsequence_iterator(burst::iterator::end_tag, subsequences_begin);

        CHECK(subsequences_begin == subsequences_end);
    }

    TEST_CASE("Одноэлементная последовательность содержит ровно одну подпоследовательность, равную "
        "всей последовательности")
    {
        const auto sequence = std::list<int>{3};

        const auto subsequence = burst::make_subsequence_iterator(sequence);
        const auto subsequences_end =
            burst::make_subsequence_iterator(burst::iterator::end_tag, subsequence);

        const auto expected_subsequence = std::vector<int>{3};
        CHECK(*subsequence == expected_subsequence);
        CHECK(std::distance(subsequence, subsequences_end) == 1);
    }

    TEST_CASE("Количество подпоследовательностей равно 2 ^ N - 1, где N — длина последовательности")
    // Пустая подпоследовательность считается концом, а потому не входит в число "действительных"
    // подпоследовательностей.
    {
        const auto sequence = {1, 1, 1, 1, 1, 1};

        const auto subsequences_begin = burst::make_subsequence_iterator(sequence);
        const auto subsequences_end =
            burst::make_subsequence_iterator(burst::iterator::end_tag, subsequences_begin);

        const auto expected_subsequence_count = burst::intpow(2, sequence.size()) - 1;
        CHECK(std::distance(subsequences_begin, subsequences_end) == expected_subsequence_count);
    }
}
