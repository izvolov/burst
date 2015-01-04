#include <vector>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/range/merge.hpp>

BOOST_AUTO_TEST_SUITE(merge)
    BOOST_AUTO_TEST_CASE(merge_function_accepts_inplace_initializer_list)
    {
        std::vector<int> even{2, 4, 6};
        std::vector<int>  odd{1, 3, 5};

        auto merged_range = burst::merge
        ({
            boost::make_iterator_range(even),
            boost::make_iterator_range(odd)
        });

        std::vector<int> all{1, 2, 3, 4, 5, 6};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            all.begin(), all.end(),
            merged_range.begin(), merged_range.end()
        );
    }
BOOST_AUTO_TEST_SUITE_END()
