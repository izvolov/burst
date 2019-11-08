#include <burst/functional/identity.hpp>
#include <burst/integer/intlog2.hpp>
#include <burst/tuple/by.hpp>
#include <utility/caller_dummies.hpp>

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <functional>
#include <string>
#include <tuple>

BOOST_AUTO_TEST_SUITE(by)
    BOOST_AUTO_TEST_CASE(transforms_specified_element_of_a_tuple)
    {
        const auto t =
            burst::by<0>([] (auto x) {return x * x;}, std::make_tuple(5, 6));
        BOOST_CHECK_EQUAL(std::get<0>(t), 25);
    }

    BOOST_AUTO_TEST_CASE(lifts_function_to_transform_specified_element_of_a_tuple)
    {
        const auto f = burst::by<0>([] (auto x) {return x * x;});
        const auto t = f(std::make_tuple(5, 2.71));
        BOOST_CHECK_EQUAL(std::get<0>(t), 25);
    }

    BOOST_AUTO_TEST_CASE(elements_other_than_specified_are_not_modified)
    {
        const auto t = std::make_tuple(5, 2.71, std::string("qwe"));
        const auto f = burst::by<1>([] (auto x) {return x * x;}, t);

        BOOST_CHECK_EQUAL(std::get<0>(f), 5);
        BOOST_CHECK_EQUAL(std::get<2>(f), "qwe");
    }

    BOOST_AUTO_TEST_CASE(resulting_tuple_does_not_refer_to_initial_tuple)
    {
        auto initial = std::make_tuple(42, "qwe", 3.14);

        auto resulting = burst::by<0>([] (auto x) {return x + 2;}, initial);
        std::get<1>(resulting) = "asd";

        BOOST_CHECK_EQUAL(std::get<1>(initial), "qwe");
    }

    enum struct initialization_way
    {
        construction,
        copy,
        move
    };

    struct movable
    {
        movable ():
            initialized_by{initialization_way::construction}
        {
        }

        movable (const movable &):
            initialized_by{initialization_way::copy}
        {
        }

        movable (movable &&):
            initialized_by{initialization_way::move}
        {
        }

        ~movable ()
        {
        }

        initialization_way initialized_by;
    };

    BOOST_AUTO_TEST_CASE(elements_of_temporary_tuple_are_moved)
    {
        movable m;

        std::tuple<movable, movable> r =
            burst::by<0>(burst::identity, std::make_tuple(m, m));

        BOOST_CHECK(std::get<0>(r).initialized_by == initialization_way::move);
        BOOST_CHECK(std::get<1>(r).initialized_by == initialization_way::move);
    }

    BOOST_AUTO_TEST_CASE(untouched_references_are_forwarded_as_references)
    {
        auto x = 5;
        auto y = 7;
        auto initial = std::make_tuple(std::ref(x), 123, std::cref(y));

        auto resulting = burst::by<1>([] (auto x) {return std::to_string(x);}, initial);
        std::get<0>(resulting) = 555;

        BOOST_CHECK_EQUAL(x, 555);

        y = 777;
        BOOST_CHECK_EQUAL(std::get<2>(resulting), 777);
    }

    BOOST_AUTO_TEST_CASE(references_are_forwarded)
    {
        auto t = std::make_tuple(5, std::string("qwe"));
        auto initial = std::make_tuple(1, 3.14, std::ref(t), true);

        auto reference_to_first_tuple_element =
            [] (auto & t)
                -> decltype(auto)
            {
                return std::get<0>(t);
            };
        auto resulting = burst::by<2>(reference_to_first_tuple_element, initial);
        std::get<2>(resulting) = 555;

        BOOST_CHECK_EQUAL(std::get<0>(t), 555);
    }

    struct dummy
    {
        dummy ()
        {
            ++instances_count;
        }

        dummy (const dummy &)
        {
            ++instances_count;
        }

        dummy (dummy &&)
        {
            ++instances_count;
        }

        ~dummy ()
        {
            --instances_count;
        }

        static int instances_count;
    };

    int dummy::instances_count = 0;

    BOOST_AUTO_TEST_CASE(passed_function_is_stored_inside)
    {
        const auto old_instances_count = dummy::instances_count;

        auto b = burst::by<0>(dummy{});
        static_cast<void>(b);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count + 1);
    }

    BOOST_AUTO_TEST_CASE(function_is_not_stored_if_passed_by_ref)
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto b = burst::by<0>(std::ref(d));
        static_cast<void>(b);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count);
    }

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_const_lvalue_when_by_is_const_lvalue)
    {
        auto calls = std::size_t{0};
        const auto by = burst::by<0>(utility::const_lvalue_call_counter(calls));

        by(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 1);
    }

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_lvalue_when_by_is_lvalue)
    {
        auto calls = std::size_t{0};
        auto by = burst::by<0>(utility::lvalue_call_counter(calls));

        by(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 1);
    }

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_rvalue_when_by_is_rvalue)
    {
        auto calls = std::size_t{0};
        auto c = utility::rvalue_call_counter(calls);

        burst::by<0>(c)(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 1);
    }

    BOOST_AUTO_TEST_CASE(referenced_function_invokes_as_lvalue)
    {
        auto calls = std::size_t{0};
        auto f = utility::lvalue_call_counter(calls);

        burst::by<0>(std::ref(f))(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 1);
    }

    BOOST_AUTO_TEST_CASE(const_referenced_function_invokes_as_const_lvalue)
    {
        auto calls = std::size_t{0};
        const auto f = utility::const_lvalue_call_counter(calls);

        burst::by<0>(std::ref(f))(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 1);
    }

    BOOST_AUTO_TEST_CASE(is_a_constexpr_function)
    {
        constexpr auto t = std::make_tuple(0, 256, 1024);
        constexpr auto l = burst::by<1>(&burst::intlog2<int>);
        constexpr auto r = l(t);
        BOOST_CHECK_EQUAL(std::get<1>(r), 8);
    }
BOOST_AUTO_TEST_SUITE_END()
