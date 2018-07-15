#include <burst/integer/intpow.hpp>
#include <burst/iterator/subsequence_iterator.hpp>
#include <utility/io/vector.hpp>

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <list>
#include <vector>

BOOST_AUTO_TEST_SUITE(subsequence_iterator)
    BOOST_AUTO_TEST_CASE(subsequence_iterator_end_is_created_using_special_tag)
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
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            subsequences_begin, subsequences_end,
            std::begin(expected_subsequences), std::end(expected_subsequences)
        );
    }

    BOOST_AUTO_TEST_CASE(empty_sequence_has_no_subsequences)
    {
        const auto sequence = std::vector<int>{};

        const auto subsequences_begin = burst::make_subsequence_iterator(sequence);
        const auto subsequences_end =
            burst::make_subsequence_iterator(burst::iterator::end_tag, subsequences_begin);

        BOOST_CHECK(subsequences_begin == subsequences_end);
    }

    BOOST_AUTO_TEST_CASE(singleton_sequence_has_one_subsequence_equal_to_whole_sequence)
    {
        const auto sequence = std::list<int>{3};

        const auto subsequence = burst::make_subsequence_iterator(sequence);
        const auto subsequences_end =
            burst::make_subsequence_iterator(burst::iterator::end_tag, subsequence);

        const auto expected_subsequence = std::vector<int>{3};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            subsequence->begin(), subsequence->end(),
            expected_subsequence.begin(), expected_subsequence.end()
        );
        BOOST_CHECK_EQUAL(std::distance(subsequence, subsequences_end), 1);
    }

    BOOST_AUTO_TEST_CASE(the_subsequence_count_is_2_in_the_power_of_sequence_length_minus_one)
    // Пустая подпоследовательность считается концом, а потому не входит в число "действительных"
    // подпоследовательностей.
    {
        const auto sequence = {1, 1, 1, 1, 1, 1};

        const auto subsequences_begin = burst::make_subsequence_iterator(sequence);
        const auto subsequences_end =
            burst::make_subsequence_iterator(burst::iterator::end_tag, subsequences_begin);

        const auto expected_subsequence_count = burst::intpow(2, sequence.size()) - 1;
        BOOST_CHECK_EQUAL
        (
            std::distance(subsequences_begin, subsequences_end),
            expected_subsequence_count
        );
    }
BOOST_AUTO_TEST_SUITE_END()
