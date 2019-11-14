#include <burst/container/make_list.hpp>
#include <burst/container/make_set.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/difference.hpp>

#include <doctest/doctest.h>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/set_algorithm.hpp>

#include <functional>
#include <vector>

TEST_SUITE("difference")
{
    TEST_CASE("difference_of_two_ranges_is_set_of_elements_in_first_range_but_not_in_second_range")
    {
        const auto natural = burst::make_vector({1, 2, 3, 4, 5, 6});
        const auto     odd = burst::make_vector({1,    3,    5   });
        //                                          ^     ^     ^

        const auto difference = burst::difference(natural, odd);

        const auto even = burst::make_vector({2, 4, 6});
        CHECK(difference == even);
    }

    TEST_CASE("accepts_multisets")
    {
        const auto    minuend = burst::make_vector({1, 1, 1, 2, 2, 3});
        const auto subtrahend = burst::make_vector({1, 1,    2      });
        //                                                ^     ^  ^

        const auto difference = burst::difference(minuend, subtrahend);

        const auto expected = burst::make_vector({1, 2, 3});
        CHECK(difference == expected);
    }

    TEST_CASE("set_minus_itself_is_empty_set")
    {
        const auto set = burst::make_vector({'a', 'b', 'c'});

        const auto difference = burst::difference(set, set);

        CHECK(difference.empty());
    }

    TEST_CASE("a_set_minus_empty_set_is_initial_set")
    {
        const auto set = burst::make_vector({3, 2, 1});
        const auto empty_set = std::vector<int>{};

        const auto difference = burst::difference(set, empty_set, std::greater<>{});

        CHECK(difference == set);
    }

    TEST_CASE("empty_set_minus_any_set_is_empty_set")
    {
        const auto empty_set = std::vector<int>{};
        const auto set = burst::make_set({1, 2, 3});

        const auto difference = burst::difference(empty_set, set);

        CHECK(difference.empty());
    }

    TEST_CASE("difference_of_saw_toothed_ranges_results_minuend")
    {
        const auto    minuend = burst::make_list({'h',      'f',      'd',      'b'     });
        const auto subtrahend = burst::make_list({     'g',      'e',      'c',      'a'});
        //                                         ^         ^         ^         ^

        const auto difference = burst::difference(minuend, subtrahend, std::greater<>{});

        CHECK(difference == minuend);
    }

    TEST_CASE("difference_equals_minuend_if_subtrahend_surrounds_minuend")
    {
        const auto    minuend =   burst::make_list({       4, 6, 8       });
        const auto subtrahend = burst::make_vector({-1, 2,         10, 11});
        //                                                 ^  ^  ^

        const auto difference = burst::difference(minuend, subtrahend);

        CHECK(difference == minuend);
    }

    TEST_CASE("accepts_ranges_of_different_types")
    {
        const auto    minuend =   burst::make_list({0,    4,    6,    7, 9, 10    });
        const auto subtrahend = burst::make_vector({0, 2,    5, 6, 6,       10, 11});
        //                                                ^           ^  ^

        const auto difference = burst::difference(minuend, subtrahend);

        const auto expected = burst::make_vector({4, 7, 9});
        CHECK(difference == expected);
    }

    TEST_CASE("resulting_range_is_writable")
    {
        auto    minuend = burst::make_vector({1,    3,    5, 7, 9});
        auto subtrahend = burst::make_vector({   2, 3, 4, 5      });
        //                                    ^              ^  ^

        auto difference = burst::difference(minuend, subtrahend);

        boost::for_each(difference, [] (auto & x) { x *= 2; });

        const auto expected = burst::make_vector({2, 14, 18});
        CHECK(difference == expected);
    }
}
