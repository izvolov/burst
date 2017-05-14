#include <burst/functional/mem_fn.hpp>

#include <boost/test/unit_test.hpp>

#include <type_traits>

BOOST_AUTO_TEST_SUITE(mem_fn_macro)
    struct dummy
    {
        const int & member_function () const
        {
            return x;
        }

        int & member_function ()
        {
            return x;
        }

        int member_function (int y) const
        {
            return x + y;
        }

        int x;
    };

    BOOST_AUTO_TEST_CASE(generates_a_function_object_calling_member_function_of_a_class)
    {
        auto call_mem_fn = BURST_MEM_FN(member_function);
        BOOST_CHECK_EQUAL(call_mem_fn(dummy{17}), 17);
    }

    BOOST_AUTO_TEST_CASE(is_composable)
    {
        const auto square_of_mem_fn = BURST_MEM_FN(member_function) | [] (auto x) {return x * x;};
        const auto d = dummy{5};
        BOOST_CHECK_EQUAL(square_of_mem_fn(d), 25);
    }

    BOOST_AUTO_TEST_CASE(returns_a_const_reference_if_object_is_const)
    {
        const auto mem_fn_of = BURST_MEM_FN(member_function);
        const auto d = dummy{42};

        decltype(auto) m = mem_fn_of(d);

        BOOST_CHECK((std::is_same<decltype(m), const int &>::value));
    }

    BOOST_AUTO_TEST_CASE(returns_a_non_const_reference_if_object_is_non_const)
    {
        const auto mem_fn_of = BURST_MEM_FN(member_function);
        auto d = dummy{42};

        mem_fn_of(d) = 17;

        BOOST_CHECK_EQUAL(d.member_function(), 17);
    }

    BOOST_AUTO_TEST_CASE(allows_overloads)
    {
        const auto mem_fn_of = BURST_MEM_FN(member_function);
        const auto d = dummy{3};
        BOOST_CHECK_EQUAL(mem_fn_of(d, 4), 7);
    }
BOOST_AUTO_TEST_SUITE_END()
