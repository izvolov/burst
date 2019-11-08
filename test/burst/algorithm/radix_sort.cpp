#include <burst/algorithm/radix_sort.hpp>
#include <utility/io/pair.hpp>

#include <doctest/doctest.h>

#include <boost/iterator/indirect_iterator.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>

#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

TEST_SUITE("radix_sort")
{
    TEST_CASE("Сортировка пустого контейнера ничего не делает")
    {
        std::vector<std::size_t> values;

        std::vector<std::size_t> buffer;
        auto sorted_values = values;
        burst::radix_sort(sorted_values.begin(), sorted_values.end(), buffer.begin());

        CHECK(sorted_values == values);
    }

    TEST_CASE("Сортировка отсортированного диапазона не изменяет его")
    {
        std::vector<std::uint8_t> initial{0, 1, 2, 3, 4};

        std::vector<std::uint8_t> buffer(initial.size());
        auto sorted = initial;
        burst::radix_sort(sorted.begin(), sorted.end(), buffer.begin());

        CHECK(sorted == initial);
    }

    TEST_CASE("Сортировка неупорядоченного одноразрядного диапазона упорядочивает его")
    {
        std::vector<std::uint8_t> numbers{2, 1, 3, 0, 4};

        std::vector<std::uint8_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin());

        auto expected = std::vector<std::uint8_t>{0, 1, 2, 3, 4};
        CHECK(numbers == expected);
    }

    TEST_CASE("Сортировка убывающего диапазона делает его возрастающим")
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

        CHECK(std::equal(descending.rbegin(), descending.rend(), ascending.begin()));
    }

    TEST_CASE("Сортировка неупорядоченного диапазона упорядочивает его")
    {
        std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};

        std::vector<std::uint32_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin());

        std::vector<std::uint32_t> expected{0, 42, 1000, 99999, 100500};
        CHECK(numbers == expected);
    }

    TEST_CASE("Возможна сортировка по убыванию")
    {
        std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};

        std::vector<std::uint32_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin(),
            [] (auto x)
            {
                return ~x;
            });

        std::vector<std::uint32_t> expected{100500, 99999, 1000, 42, 0};
        CHECK(numbers ==expected);
    }

    TEST_CASE("Возможна побитовая сортировка")
    {
        std::vector<std::uint8_t> numbers{0, 5, 3, 7, 1, 2, 4, 6};

        std::vector<std::uint8_t> buffer(numbers.size());
        burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin(), burst::identity,
            [] (const std::uint8_t & number) -> bool
            {
                return number & 0x01;
            });

        std::vector<std::uint8_t> even_goes_first{0, 1, 2, 3, 4, 5, 6, 7};
        CHECK(numbers == even_goes_first);
    }

    TEST_CASE("Экстремальные значения сортируются правильно")
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
        CHECK(numbers == expected);
    }

    TEST_CASE("Алгоритм сортировки устойчив")
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
        CHECK(numbers == expected);
    }

    TEST_CASE("Умеет сортировать знаковые числа")
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
        CHECK(values == expected);
    }

    TEST_CASE("Возможно сортировать некопируемые объекты")
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
        CHECK(std::equal
        (
            boost::make_indirect_iterator(std::begin(pointers)),
            boost::make_indirect_iterator(std::end(pointers)),
            std::begin(expected)
        ));
    }

    TEST_CASE("Возможно сортировать некопируемые одноразрядные объекты")
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
        CHECK(std::equal
        (
            boost::make_indirect_iterator(std::begin(pointers)),
            boost::make_indirect_iterator(std::end(pointers)),
            std::begin(expected)
        ));
    }

    template <typename Integer>
    struct implicitly_nonmovable
    {

        explicit implicitly_nonmovable (Integer n):
            n(n)
        {
        }

        implicitly_nonmovable () = default;

        implicitly_nonmovable (const implicitly_nonmovable &) = default;
        implicitly_nonmovable & operator = (const implicitly_nonmovable &) = default;

        ~implicitly_nonmovable () = default;

        Integer n;
    };

    TEST_CASE("Может сортировать объекты с неявно удалённым операциями переноса")
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

        explicit explicitly_nonmovable (Integer n):
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

    TEST_CASE("Может сортировать объекты с явно удалённым операциями переноса")
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

    TEST_CASE("Допускает на вход диапазоны")
    {
        std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};

        std::vector<std::uint32_t> buffer(numbers.size());
        burst::radix_sort(numbers, buffer.begin());

        std::vector<std::uint32_t> expected{0, 42, 1000, 99999, 100500};
        CHECK(numbers == expected);
    }
}
