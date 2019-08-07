#include <burst/container/make_set.hpp>
#include <utility/io/set.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

#include <set>
#include <sstream>
#include <type_traits>
#include <vector>

namespace std
{
    template <typename T, typename C, typename A, typename Coll>
    bool operator == (const std::set<T, C, A> & s, const Coll & l)
    {
        return std::mismatch(s.begin(), s.end(), l.begin(), l.end()) == std::make_pair(s.end(), l.end());
    }
} // namespace std

TEST_SUITE("make_set")
{
    TEST_CASE("value_type_of_created_set_is_taken_from_value_type_of_incoming_range")
    {
        const auto range = boost::irange(0, 4);
        const auto set = burst::make_set(range);
        CHECK
        (
            std::is_same
            <
                decltype(set)::value_type,
                boost::range_value<decltype(range)>::type
            >
            ::value
        );
    }

    TEST_CASE("accepts_range_by_rvalue")
    {
        const auto set = burst::make_set(boost::irange<std::uint32_t>(0, 6));
        CHECK(set == std::set<std::uint32_t>{0, 1, 2, 3, 4, 5});
    }

    TEST_CASE("accepts_range_by_lvalue")
    {
        std::stringstream stream("5 4 3 2");
        const auto range =
            boost::make_iterator_range
            (
                std::istream_iterator<int>(stream),
                std::istream_iterator<int>{}
            );

        const auto set = burst::make_set(range);

        const auto expected = {2, 3, 4, 5};
        CHECK(set == expected);
    }

    TEST_CASE("accepts_containers")
    {
        const auto v = std::vector<unsigned>{1u, 2u, 3u};

         const auto set = burst::make_set(v);

        const auto expected = {1u, 2u, 3u};
        CHECK(set == expected);
    }

    TEST_CASE("accepts_rvalue_containers")
    {
         const auto set = burst::make_set(std::vector<unsigned>{1u, 1u, 1u});

        const auto expected = {1u};
        CHECK(set == expected);
    }

    TEST_CASE("accepts_custom_compare")
    {
        const auto set = burst::make_set(boost::irange(0, 6), std::greater<>{});

        const auto expected = {5, 4, 3, 2, 1, 0};
        CHECK(set == expected);
    }

    TEST_CASE("accepts_initializer_list")
    {
        const auto set = burst::make_set({'3', '2', '0', '4', '1'});

        const auto expected = std::string("01234");
        CHECK(set == expected);
    }

    TEST_CASE("value_type_may_be_specified_explicitly_when_constructed_from_range")
    {
        const auto set = burst::make_set<std::size_t>(boost::irange<int>(0, 4));
        CHECK
        (
            std::is_same
            <
                decltype(set)::value_type,
                std::size_t
            >
            ::value
        );
    }
}
