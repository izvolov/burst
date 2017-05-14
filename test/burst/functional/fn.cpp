#include <burst/functional/fn.hpp>

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(fn_macro)
    struct dummy
    {
        int member;
    };

    const int & free_function (const dummy & d)
    {
        return d.member;
    }

    int & free_function (dummy & d)
    {
        return d.member;
    }

    int free_function (const dummy & d, int y)
    {
        return d.member + y;
    }

    BOOST_AUTO_TEST_CASE(generates_a_function_object_calling_free_function_over_a_class)
    {
        auto call_fn = BURST_FN(free_function);
        BOOST_CHECK_EQUAL(call_fn(dummy{17}), 17);
    }

    BOOST_AUTO_TEST_CASE(returns_a_const_reference_if_object_is_const)
    {
        const auto fn = BURST_FN(free_function);
        const auto d = dummy{42};

        decltype(auto) m = fn(d);

        BOOST_CHECK((std::is_same<decltype(m), const int &>::value));
    }

    BOOST_AUTO_TEST_CASE(returns_a_mutable_reference_if_object_is_mutable)
    {
        const auto fn = BURST_FN(free_function);
        auto d = dummy{42};

        fn(d) = 17;

        BOOST_CHECK_EQUAL(free_function(d), 17);
    }

    BOOST_AUTO_TEST_CASE(allows_overloads)
    {
        const auto fn = BURST_FN(free_function);
        const auto d = dummy{3};
        BOOST_CHECK_EQUAL(fn(d, 4), 7);
    }

    BOOST_AUTO_TEST_CASE(is_composable)
    {
        const auto square_of_abs = BURST_FN(std::abs) | [] (auto x) {return x * x;};
        BOOST_CHECK_EQUAL(square_of_abs(-5), 25);
    }
BOOST_AUTO_TEST_SUITE_END()
