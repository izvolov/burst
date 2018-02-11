#include <burst/functional/identity.hpp>
#include <burst/integer/intlog2.hpp>
#include <burst/tuple/by_all.hpp>
#include <utility/io/tuple.hpp>

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <functional>
#include <tuple>

BOOST_AUTO_TEST_SUITE(by_all)
    BOOST_AUTO_TEST_CASE(transforms_all_elements_of_a_tuple)
    {
        const auto t =
            burst::by_all([] (auto x) {return x * x;}, std::make_tuple(5, 6));
        BOOST_CHECK_EQUAL(t, std::make_tuple(25, 36));
    }

    BOOST_AUTO_TEST_CASE(lifts_function_to_transform_all_elements_of_a_tuple)
    {
        const auto f = burst::by_all([] (auto x) {return x * x;});
        const auto t = f(std::make_tuple(5, 7));
        BOOST_CHECK_EQUAL(t, std::make_tuple(25, 49));
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
            burst::by_all(burst::identity, std::make_tuple(m, m));

        BOOST_CHECK(std::get<0>(r).initialized_by == initialization_way::move);
        BOOST_CHECK(std::get<1>(r).initialized_by == initialization_way::move);
    }

    BOOST_AUTO_TEST_CASE(references_are_forwarded)
    {
        auto t1 = std::make_tuple(5, std::string("qwe"));
        auto t2 = std::make_tuple(std::string("asd"), 2.71);
        auto initial = std::make_tuple(std::ref(t1), std::ref(t2));

        const auto reference_to_first_tuple_element =
            [] (auto & t)
                -> decltype(auto)
            {
                return std::get<0>(t);
            };
        auto resulting = burst::by_all(reference_to_first_tuple_element, initial);
        std::get<1>(resulting) = "fgh";

        BOOST_CHECK_EQUAL(std::get<0>(t2), "fgh");
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

        auto b = burst::by_all(dummy{});
        static_cast<void>(b);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count + 1);
    }

    BOOST_AUTO_TEST_CASE(function_is_not_stored_if_passed_by_ref)
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto b = burst::by_all(std::ref(d));
        static_cast<void>(b);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count);
    }

    struct const_lvalue_caller
    {
        const_lvalue_caller (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T>
        auto operator () (T t) const &
        {
            ++calls;
            return t;
        }

        template <typename T>
        auto operator () (T t) & = delete;

        template <typename T>
        auto operator () (T t) && = delete;

        std::size_t & calls;
    };

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_const_lvalue_when_by_all_is_const_lvalue)
    {
        auto calls = std::size_t{0};
        const auto b = burst::by_all(const_lvalue_caller{calls});

        b(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 2);
    }

    struct lvalue_caller
    {
        lvalue_caller (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T>
        auto operator () (T t) const & = delete;

        template <typename T>
        auto operator () (T t) &
        {
            ++calls;
            return t;
        }

        template <typename T>
        auto operator () (T t) && = delete;

        std::size_t & calls;
    };

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_lvalue_when_by_all_is_lvalue)
    {
        auto calls = std::size_t{0};
        auto b = burst::by_all(lvalue_caller{calls});

        b(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 2);
    }

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_lvalue_when_by_all_is_rvalue)
    {
        auto calls = std::size_t{0};
        auto l = lvalue_caller{calls};

        burst::by_all(l)(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 2);
    }

    BOOST_AUTO_TEST_CASE(referenced_function_invokes_as_lvalue)
    {
        auto calls = std::size_t{0};
        auto f = lvalue_caller{calls};

        burst::by_all(std::ref(f))(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 2);
    }

    BOOST_AUTO_TEST_CASE(const_referenced_function_invokes_as_const_lvalue)
    {
        auto calls = std::size_t{0};
        const auto f = const_lvalue_caller{calls};

        burst::by_all(std::ref(f))(std::make_tuple(1, "qwe"));

        BOOST_CHECK_EQUAL(calls, 2);
    }

    BOOST_AUTO_TEST_CASE(is_a_constexpr_function)
    {
        constexpr auto t = std::make_tuple(1, 256, 1024);
        constexpr auto l = burst::by_all(&burst::intlog2<int>);
        constexpr auto r = l(t);
        static_assert(r == std::make_tuple(0, 8, 10), "");
    }
BOOST_AUTO_TEST_SUITE_END()
