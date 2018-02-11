#include <burst/algorithm/radix_sort.hpp>
#include <utility/io/pair.hpp>

#include <boost/iterator/indirect_iterator.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>
#include <boost/test/unit_test.hpp>

#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

BOOST_AUTO_TEST_SUITE(radix_sort)
    BOOST_AUTO_TEST_CASE(sorting_empty_range_does_nothing)
    {
        std::vector<std::size_t> values;

        std::vector<std::size_t> buffer;
        auto sorted_values = values;
        burst::radix_sort(sorted_values.begin(), sorted_values.end(), buffer.begin());

        BOOST_CHECK(sorted_values == values);
    }

    BOOST_AUTO_TEST_CASE(sorting_already_sorted_range_results_the_same_range)
    {
        std::vector<std::uint8_t> initial{0, 1, 2, 3, 4};

        std::vector<std::uint8_t> buffer(initial.size());
        auto sorted = initial;
        burst::radix_sort(sorted.begin(), sorted.end(), buffer.begin());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted), boost::end(sorted),
            boost::begin(initial), boost::end(initial)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_chaotic_single_byte_range_results_sorted_range)
    {
        std::vector<std::uint8_t> numbers{2, 1, 3, 0, 4};

        std::vector<std::uint8_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin());

        auto expected = {0, 1, 2, 3, 4};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_descending_range_results_ascending_range)
    {
        std::vector<std::string> descending{"1000", "100", "10", "1"};

        std::vector<std::string> buffer(descending.size());
        std::vector<std::string> ascending(descending.begin(), descending.end());
        burst::radix_sort(ascending.begin(), ascending.end(), buffer.begin(),
            [] (const std::string & string)
            {
                return string.size();
            }
        );

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::rbegin(descending), boost::rend(descending),
            boost::begin(ascending), boost::end(ascending)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_chaotic_range_results_sorted_range)
    {
        std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};

        std::vector<std::uint32_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin());

        std::vector<std::uint32_t> expected{0, 42, 1000, 99999, 100500};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_in_descending_order)
    {
        std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};

        std::vector<std::uint32_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin(),
            [] (auto x)
            {
                return ~x;
            });

        std::vector<std::uint32_t> expected{100500, 99999, 1000, 42, 0};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_bitwise)
    {
        std::vector<std::uint8_t> numbers{0, 5, 3, 7, 1, 2, 4, 6};

        std::vector<std::uint8_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin(), burst::identity,
            [] (const std::uint8_t & number) -> bool
            {
                return number & 0x01;
            });

        std::vector<std::uint8_t> even_goes_first{0, 1, 2, 3, 4, 5, 6, 7};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(even_goes_first), boost::end(even_goes_first)
        );
    }

    BOOST_AUTO_TEST_CASE(extreme_values_are_sorted_properly)
    {
        std::vector<std::size_t> numbers
        {
            0,
            std::numeric_limits<std::size_t>::min(),
            std::numeric_limits<std::size_t>::max()
        };

        std::vector<std::size_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin());

        std::vector<std::size_t> expected
        {
            std::numeric_limits<std::size_t>::min(),
            0,
            std::numeric_limits<std::size_t>::max()
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(sorting_algorithm_is_stable)
    {
        std::vector<std::pair<std::uint16_t, std::string>> numbers
        {
            {1, "раз"},
            {1, "два"},
            {0, "три"},
            {0, "четыре"},
            {1, "пять"}
        };

        std::vector<std::pair<std::uint16_t, std::string>> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin(),
            [] (const auto & pair)
            {
                return pair.first;
            });

        std::vector<std::pair<std::uint16_t, std::string>> expected
        {
            {0, "три"},
            {0, "четыре"},
            {1, "раз"},
            {1, "два"},
            {1, "пять"}
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_signed_values)
    {
        std::vector<std::int64_t> values
        {
            0,
            -1,
            1,
            -2,
            2,
            -100500,
            100500,
            std::numeric_limits<std::int64_t>::min(),
            std::numeric_limits<std::int64_t>::max()
        };

        std::vector<std::int64_t> buffer(values.size());
        burst::radix_sort(values.begin(), values.end(), buffer.begin());

        std::vector<std::int64_t> expected
        {
            std::numeric_limits<std::int64_t>::min(),
            -100500,
            -2,
            -1,
            0,
            1,
            2,
            100500,
            std::numeric_limits<std::int64_t>::max()
        };
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(values), std::end(values),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_noncopyable_objects)
    {
        std::vector<std::unique_ptr<std::int64_t>> pointers;
        pointers.emplace_back(std::make_unique<std::int64_t>(30));
        pointers.emplace_back(std::make_unique<std::int64_t>(5));
        pointers.emplace_back(std::make_unique<std::int64_t>(-100500));
        pointers.emplace_back(std::make_unique<std::int64_t>(20152016));
        pointers.emplace_back(std::make_unique<std::int64_t>(0));

        std::vector<std::unique_ptr<std::int64_t>> buffer(pointers.size());
        burst::radix_sort(pointers.begin(), pointers.end(), buffer.begin(),
            [] (const auto & p)
            {
                return *p;
            });

        auto expected = {-100500, 0, 5, 30, 20152016};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::make_indirect_iterator(std::begin(pointers)), boost::make_indirect_iterator(std::end(pointers)),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(can_sort_noncopyable_single_byte_objects)
    {
        std::vector<std::unique_ptr<std::int8_t>> pointers;
        pointers.emplace_back(std::make_unique<std::int8_t>(0x30));
        pointers.emplace_back(std::make_unique<std::int8_t>(0x5));
        pointers.emplace_back(std::make_unique<std::int8_t>(-0xf));
        pointers.emplace_back(std::make_unique<std::int8_t>(0x7f));
        pointers.emplace_back(std::make_unique<std::int8_t>(0));

        std::vector<std::unique_ptr<std::int8_t>> buffer(pointers.size());
        burst::radix_sort(pointers.begin(), pointers.end(), buffer.begin(),
            [] (const auto & p)
            {
                return *p;
            });

        auto expected = {-0xf, 0, 0x5, 0x30, 0x7f};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::make_indirect_iterator(std::begin(pointers)), boost::make_indirect_iterator(std::end(pointers)),
            std::begin(expected), std::end(expected)
        );
    }

    template <typename Integer>
    struct implicitly_nonmovable
    {

        implicitly_nonmovable (Integer n):
            n(n)
        {
        }

        implicitly_nonmovable () = default;

        implicitly_nonmovable (const implicitly_nonmovable &) = default;
        implicitly_nonmovable & operator = (const implicitly_nonmovable &) = default;

        ~implicitly_nonmovable () = default;

        Integer n;
    };

    BOOST_AUTO_TEST_CASE(can_sort_implicitly_nonmovable_objects)
    {
        using object_type = implicitly_nonmovable<std::int64_t>;
        auto objects =
            std::vector<object_type>
            {
                object_type(100500),
                object_type(-200),
                object_type(0)
            };

        std::vector<object_type> buffer(objects.size());
        burst::radix_sort(objects, buffer.begin(), [] (auto && n) {return n.n;});
    }

    template <typename Integer>
    struct explicitly_nonmovable
    {

        explicitly_nonmovable (Integer n):
            n(n)
        {
        }

        explicitly_nonmovable () = default;

        explicitly_nonmovable (explicitly_nonmovable &&) = delete;
        explicitly_nonmovable & operator = (explicitly_nonmovable &&) = delete;

        explicitly_nonmovable (const explicitly_nonmovable &) = default;
        explicitly_nonmovable & operator = (const explicitly_nonmovable &) = default;

        ~explicitly_nonmovable () = default;

        Integer n;
    };

    BOOST_AUTO_TEST_CASE(can_sort_explicitly_nonmovable_objects)
    {
        using object_type = explicitly_nonmovable<std::int8_t>;
        std::vector<object_type> objects;
        object_type x1(100);
        objects.push_back(x1);
        object_type x2(-20);
        objects.push_back(x2);
        object_type x3(0);
        objects.push_back(x3);

        std::vector<object_type> buffer(objects.size());
        burst::radix_sort(objects, buffer.begin(), [] (auto && n) {return n.n;});
    }

    BOOST_AUTO_TEST_CASE(works_with_ranges)
    {
        std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};

        std::vector<std::uint32_t> buffer(numbers.size());
        burst::radix_sort(numbers, buffer.begin());

        std::vector<std::uint32_t> expected{0, 42, 1000, 99999, 100500};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(numbers), boost::end(numbers),
            boost::begin(expected), boost::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
