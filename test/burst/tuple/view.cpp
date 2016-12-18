#include <burst/tuple/view.hpp>
#include <test/output/tuple.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/tuple/tuple.hpp>

#include <string>
#include <tuple>

BOOST_AUTO_TEST_SUITE(view)
    BOOST_AUTO_TEST_CASE(gets_subview_of_a_tuple)
    {
        const auto t = std::make_tuple('a', 'b', 'c');
        BOOST_CHECK_EQUAL
        (
            (burst::view<0, 1>(t)),
            (std::make_tuple('a', 'b'))
        );
    }

    BOOST_AUTO_TEST_CASE(elements_of_view_of_lvalue_tuple_are_lvalues)
    {
        auto t = std::make_tuple(42, std::string("qwe"), 3.14);

        auto l = burst::view<0, 1>(t);
        std::get<0>(l) = 17;
        std::get<1>(l).append("rty");

        BOOST_CHECK_EQUAL(std::get<0>(t), 17);
        BOOST_CHECK_EQUAL(std::get<1>(t), "qwerty");
    }

    struct dummy
    {
        dummy (const dummy &) = delete;
        dummy & operator = (const dummy &) = delete;
        dummy (dummy &&) = delete;
        dummy & operator = (dummy &&) = delete;
        ~dummy () = default;
    };

    BOOST_AUTO_TEST_CASE(neither_copies_nor_moves)
    {
        burst::view<1>(std::forward_as_tuple(1, dummy{}, 3.14));
    }

    BOOST_AUTO_TEST_CASE(is_compatible_with_boost_tuple)
    {
        const auto t = boost::make_tuple(42, 3.14, true);
        const auto l = burst::view<0, 2>(t);
        BOOST_CHECK_EQUAL(std::get<1>(l), true);
    }
BOOST_AUTO_TEST_SUITE_END()
