#include <vector>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/range/intersect.hpp>

BOOST_AUTO_TEST_SUITE(intersect)
    BOOST_AUTO_TEST_CASE(intersect_function_accepts_inplace_initializer_list)
    {
        std::vector<int> natural{1, 2, 3, 4, 5, 6};
        std::vector<int> odd{1, 3, 5};

        auto intersected_range = burst::intersect
        ({
            boost::make_iterator_range(natural),
            boost::make_iterator_range(odd)
        });

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            intersected_range.begin(), intersected_range.end(),
            odd.begin(), odd.end()
        );
    }
BOOST_AUTO_TEST_SUITE_END()
