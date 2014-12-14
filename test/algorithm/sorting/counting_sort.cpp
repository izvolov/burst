#include <vector>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>
#include <boost/test/unit_test.hpp>

#include <thrust/algorithm/sorting/counting_sort.hpp>

BOOST_AUTO_TEST_SUITE(counting_sort)
    BOOST_AUTO_TEST_CASE(sorting_empty_range_does_nothing)
    {
        std::vector<unsigned char> values;

        std::vector<unsigned char> sorted_values(values.size());
        thrust::counting_sort(values.begin(), values.end(), sorted_values.begin());

        BOOST_CHECK(values == sorted_values);
    }

    BOOST_AUTO_TEST_CASE(sorting_descending_range_results_ascending_range)
    {
        std::vector<std::size_t> descending_by_low_byte{0x0104, 0x0203, 0x0302, 0x0401};

        std::vector<std::size_t> ascending_by_low_byte(descending_by_low_byte.size());
        thrust::counting_sort
        (
            descending_by_low_byte.begin(),
            descending_by_low_byte.end(),
            ascending_by_low_byte.begin(),
            [] (const std::size_t & integer) -> unsigned char { return integer & 0xff; }
        );

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::rbegin(descending_by_low_byte), boost::rend(descending_by_low_byte),
            boost::begin(ascending_by_low_byte), boost::end(ascending_by_low_byte)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
