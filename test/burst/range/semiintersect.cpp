#include <burst/range/make_range_vector.hpp>
#include <burst/range/semiintersect.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <functional>
#include <list>
#include <string>
#include <vector>

TEST_SUITE("semiintersect")
{
    TEST_CASE("semiintersecting_empty_ranges_results_empty_range")
    {
        std::vector<int> first;
        std::vector<int> second;
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 1);

        CHECK(semiintersection.empty());
    }

    TEST_CASE("semiintersection_of_min_items_larger_than_range_count_is_empty")
    {
        auto  first = burst::make_vector({1, 2, 3, 4});
        auto second = burst::make_vector({1, 2, 3, 4});
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 10);

        CHECK(semiintersection.empty());
    }

    TEST_CASE("semiintersecting_one_range_results_the_same_range")
    {
        auto only = boost::irange(1, 5);
        auto ranges = burst::make_range_vector(only);
        auto semiintersection = burst::semiintersect(ranges, 1);

        CHECK(semiintersection == boost::irange(1, 5));
    }

    TEST_CASE("semiintersecting_several_equal_ranges_with_any_min_items_results_range_equal_to_any_of_them")
    {
        auto first = {4, 4, 5, 6, 6, 7};
        auto second = first;
        auto third = first;

        {
            auto ranges = burst::make_range_vector(first, second, third);
            auto semiintersection1 = burst::semiintersect(ranges, 1);

            CHECK(semiintersection1 == first);
        }

        {
            auto ranges = burst::make_range_vector(first, second, third);
            auto semiintersection2 = burst::semiintersect(ranges, 2);

            CHECK(semiintersection2 == second);
        }

        {
            auto ranges = burst::make_range_vector(first, second, third);
            auto semiintersection3 = burst::semiintersect(ranges, 3);

            CHECK(semiintersection3 == third);
        }
    }

    TEST_CASE("semiintersecting_two_nested_ranges_with_two_min_items_results_shortest_of_them")
    {
        std::string  long_range("abcdef");
        std::string short_range(  "cde" );
        //                         ^^^
        auto ranges = burst::make_range_vector(long_range, short_range);

        auto semiintersection = burst::semiintersect(ranges, 2);

        CHECK(semiintersection == short_range);
    }

    TEST_CASE("semiintersecting_two_nested_ranges_with_one_min_item_results_longest_of_them")
    {
        std::string short_range(   "cde" );
        std::string  long_range("aabcdef");
        //                       ^^^^^^^
        auto ranges = burst::make_range_vector(long_range, short_range);

        auto semiintersection = burst::semiintersect(ranges, 1);

        CHECK(semiintersection == long_range);
    }

    TEST_CASE("semiintersecting_saw_toothed_ranges_with_two_min_items_results_empty_range")
    {
        std::list<char>  first{'h',      'f',      'd',      'b'     };
        std::list<char> second{     'g',      'e',      'c',      'a'};
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 2, std::greater<>{});

        CHECK(semiintersection.empty());
    }

    TEST_CASE("semiintersecting_saw_toothed_ranges_with_one_min_item_results_union")
    {
        std::list<char>  first{'h',      'f',      'd',      'b'     };
        std::list<char> second{     'g',      'e',      'c',      'a'};
        //                      ^    ^    ^    ^    ^    ^    ^    ^
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 1, std::greater<>{});

        CHECK(semiintersection == std::string("hgfedcba"));
    }

    TEST_CASE("semiintersection_supports_custom_ordering")
    {
        auto  first = {   3, 3, 2, 1};
        auto second = {4, 3,    2   };
        //                ^     ^
        auto ranges = burst::make_range_vector(first, second);

        auto semiintersection = burst::semiintersect(ranges, 2, std::greater<>{});

        auto expected_collection = {3, 2};
        CHECK(semiintersection == expected_collection);
    }

    TEST_CASE("semiintersecting_overlaying_ranges_with_one_min_item_results_whole_range")
    {
        auto  first = {1, 2         };
        auto second = {   2, 3, 3   };
        auto  third = {      3, 3, 4};
        //             ^  ^  ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection = burst::semiintersect(ranges, 1);

        auto expected = {1, 2, 3, 3, 4};
        CHECK(semiintersection == expected);
    }

    TEST_CASE("semiintersecting_overlaying_ranges_with_greater_than_one_min_item_results_overlaying_part")
    {
        auto  first = {1, 2      };
        auto second = {   2, 3   };
        auto  third = {      3, 4};
        //                ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection = burst::semiintersect(ranges, 2);

        auto expected = {2, 3};
        CHECK(semiintersection == expected);
    }

    TEST_CASE("semiintersection_of_several_consecutive_sorted_ranges_with_several_min_items_is_empty")
    {
        auto  first = {1, 2, 3                  };
        auto second = {         4, 5, 6         };
        auto  third = {                  7, 8, 9};
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection = burst::semiintersect(ranges, 2, std::less<>{});

        CHECK(semiintersection.empty());
    }

    TEST_CASE("semiintersection_of_several_consecutive_sorted_ranges_with_one_min_item_is_consecutive_range")
    {
        auto  first = {1, 2, 3                  };
        auto second = {         4, 5, 6         };
        auto  third = {                  7, 8, 9};
        //             ^  ^  ^  ^  ^  ^  ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection = burst::semiintersect(ranges, 1, std::less<>{});

        auto expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        CHECK(semiintersection == expected);
    }

    TEST_CASE("repeating_elements_do_not_produce_excess_matches")
    {
        auto  first = {1, 1, 1};
        auto second = {1, 1, 1};
        auto  third = {1, 1, 1};
        //             ^  ^  ^
        auto ranges = burst::make_range_vector(first, second, third);

        auto semiintersection =
            burst::semiintersect(ranges, 2, std::less<>{});

        CHECK(semiintersection == first);
    }

    TEST_CASE("ranges_are_considered_multisets")
    {
        auto  first = {0, 0, 1,       2};
        auto second = {0,    1, 1      };
        auto  third = {      1, 1, 1, 2};
        //             ^     ^  ^     ^

        auto ranges = burst::make_range_vector(first, second, third);
        auto semiintersection = burst::semiintersect(ranges, 2);

        auto expected_collection = {0, 1, 1, 2};
        CHECK(semiintersection == expected_collection);
    }
}
