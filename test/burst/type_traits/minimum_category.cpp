#include <burst/type_traits/minimum_category.hpp>

#include <doctest/doctest.h>

#include <type_traits>
#include <iterator>

TEST_SUITE("minimum_category")
{
    TEST_CASE("Выбирает минимальную категорию итератора из набора категорий")
    {
        using input = std::input_iterator_tag;
        using forward = std::forward_iterator_tag;
        using bidir = std::bidirectional_iterator_tag;
        using random = std::random_access_iterator_tag;

        CHECK(std::is_same<burst::minimum_category_t<input, input>, input>::value);
        CHECK(std::is_same<burst::minimum_category_t<input, forward>, input>::value);
        CHECK(std::is_same<burst::minimum_category_t<input, bidir>, input>::value);
        CHECK(std::is_same<burst::minimum_category_t<input, random>, input>::value);

        CHECK(std::is_same<burst::minimum_category_t<forward, input>, input>::value);
        CHECK(std::is_same<burst::minimum_category_t<forward, forward>, forward>::value);
        CHECK(std::is_same<burst::minimum_category_t<forward, bidir>, forward>::value);
        CHECK(std::is_same<burst::minimum_category_t<forward, random>, forward>::value);

        CHECK(std::is_same<burst::minimum_category_t<bidir, input>, input>::value);
        CHECK(std::is_same<burst::minimum_category_t<bidir, forward>, forward>::value);
        CHECK(std::is_same<burst::minimum_category_t<bidir, bidir>, bidir>::value);
        CHECK(std::is_same<burst::minimum_category_t<bidir, random>, bidir>::value);

        CHECK(std::is_same<burst::minimum_category_t<random, input>, input>::value);
        CHECK(std::is_same<burst::minimum_category_t<random, forward>, forward>::value);
        CHECK(std::is_same<burst::minimum_category_t<random, bidir>, bidir>::value);
        CHECK(std::is_same<burst::minimum_category_t<random, random>, random>::value);
    }

    TEST_CASE("Принимает произвольные наборы категорий")
    {
        using input = std::input_iterator_tag;
        using forward = std::forward_iterator_tag;
        using bidir = std::bidirectional_iterator_tag;
        using random = std::random_access_iterator_tag;

        CHECK(std::is_same<burst::minimum_category_t<input, forward, bidir, random>, input>::value);
        CHECK(std::is_same<burst::minimum_category_t<forward, random, bidir>, forward>::value);
        CHECK(std::is_same<burst::minimum_category_t<bidir, random, bidir, random>, bidir>::value);
        CHECK(std::is_same<burst::minimum_category_t<random, random, random>, random>::value);
    }
}
