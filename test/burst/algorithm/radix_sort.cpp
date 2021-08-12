#include <utility/io/pair.hpp>
#include <utility/random_vector.hpp>

#include <burst/algorithm/radix_sort.hpp>

#include <doctest/doctest.h>

#include <boost/iterator/indirect_iterator.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/rbegin.hpp>
#include <boost/range/rend.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

TEST_SUITE("radix_sort")
{
    TEST_CASE("Сортировка пустого контейнера ничего не делает")
    {
        std::vector<std::size_t> values;

        std::vector<std::size_t> buffer;
        SUBCASE("при последовательной сортировке")
        {
            auto sorted_values = values;
            burst::radix_sort(sorted_values.begin(), sorted_values.end(), buffer.begin());

            CHECK(sorted_values == values);
        }
        SUBCASE("при параллельной сортировке")
        {
            auto sorted_values = values;
            burst::radix_sort
            (
                burst::par(3),
                sorted_values.begin(),
                sorted_values.end(),
                buffer.begin()
            );

            CHECK(sorted_values == values);
        }
    }

    TEST_CASE("Сортировка отсортированного диапазона не изменяет его")
    {
        std::vector<std::uint8_t> initial{0, 1, 2, 3, 4};
        std::vector<std::uint8_t> buffer(initial.size());
        SUBCASE("при последовательной сортировке")
        {
            auto sorted = initial;
            burst::radix_sort(sorted.begin(), sorted.end(), buffer.begin());

            CHECK(sorted == initial);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> initial{0, 1, 2, 3, 4};
            auto sorted = initial;
            burst::radix_sort(burst::par(2), sorted.begin(), sorted.end(), buffer.begin());

            CHECK(sorted == initial);
        }
    }

    TEST_CASE("Сортировка неупорядоченного одноразрядного диапазона упорядочивает его")
    {
        auto expected = std::vector<std::uint8_t>{0, 1, 2, 3, 4};
        std::vector<std::uint8_t> buffer(expected.size());
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint8_t> numbers{2, 1, 3, 0, 4};
            burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin());

            CHECK(numbers == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> numbers{2, 1, 3, 0, 4};
            burst::radix_sort(burst::par(3), numbers.begin(), numbers.end(), buffer.begin());

            CHECK(numbers == expected);
        }
    }

    TEST_CASE("Сортировка убывающего диапазона делает его возрастающим")
    {
        SUBCASE("при последовательной сортировке")
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
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::string> descending{"1000", "100", "10", "1"};
            std::vector<std::string> buffer(descending.size());
            std::vector<std::string> ascending(descending.begin(), descending.end());
            burst::radix_sort(burst::par(4), ascending.begin(), ascending.end(), buffer.begin(),
                [] (const std::string & string)
                {
                    return string.size();
                }
            );

            CHECK(std::equal(descending.rbegin(), descending.rend(), ascending.begin()));
        }
    }

    TEST_CASE("Сортировка неупорядоченного диапазона упорядочивает его")
    {
        std::vector<std::uint32_t> expected{0, 42, 1000, 99999, 100500};
        std::vector<std::uint32_t> buffer(expected.size());
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};
            burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin());

            CHECK(numbers == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};
            burst::radix_sort(burst::par(2), numbers.begin(), numbers.end(), buffer.begin());

            CHECK(numbers == expected);
        }
    }

    TEST_CASE("Возможна сортировка по убыванию")
    {
        std::vector<std::uint32_t> expected{100500, 99999, 1000, 42, 0};
        std::vector<std::uint32_t> buffer(expected.size());
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};
            burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin(),
                [] (auto x)
                {
                    return ~x;
                });

            CHECK(numbers == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};
            burst::radix_sort(burst::par(3), numbers.begin(), numbers.end(), buffer.begin(),
                [] (auto x)
                {
                    return ~x;
                });

            CHECK(numbers == expected);
        }
    }

    TEST_CASE("Возможна побитовая сортировка")
    {
        std::vector<std::uint8_t> even_goes_first{0, 1, 2, 3, 4, 5, 6, 7};
        std::vector<std::uint8_t> buffer(even_goes_first.size());

        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint8_t> numbers{0, 5, 3, 7, 1, 2, 4, 6};
            burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin(), burst::identity,
                [] (const std::uint8_t & number) -> bool
                {
                    return number & 0x01;
                });

            CHECK(numbers == even_goes_first);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> numbers{0, 5, 3, 7, 1, 2, 4, 6};
            burst::radix_sort
            (
                burst::par(4),
                numbers.begin(),
                numbers.end(),
                buffer.begin(),
                burst::identity,
                [] (const std::uint8_t & number) -> bool
                {
                    return number & 0x01;
                }
            );

            CHECK(numbers == even_goes_first);
        }
    }

    TEST_CASE("Экстремальные значения сортируются правильно")
    {
        const auto initial =
            std::vector<std::size_t>
            {
                0,
                std::numeric_limits<std::size_t>::min(),
                std::numeric_limits<std::size_t>::max()
            };
        std::vector<std::size_t> buffer(initial.size());

        const auto expected =
            std::vector<std::size_t>
            {
                std::numeric_limits<std::size_t>::min(),
                0,
                std::numeric_limits<std::size_t>::max()
            };

        SUBCASE("при последовательной сортировке")
        {
            auto numbers = initial;
            burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin());

            CHECK(numbers == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            auto numbers = initial;
            burst::radix_sort(burst::par(2), numbers.begin(), numbers.end(), buffer.begin());

            CHECK(numbers == expected);
        }
    }

    TEST_CASE("Алгоритм сортировки устойчив")
    {
        const auto initial =
            std::vector<std::pair<std::uint16_t, std::string>>
            {
                {1, "раз"},
                {1, "два"},
                {0, "три"},
                {0, "четыре"},
                {1, "пять"}
            };
        std::vector<std::pair<std::uint16_t, std::string>> buffer(initial.size());

        const auto expected =
            std::vector<std::pair<std::uint16_t, std::string>>
            {
                {0, "три"},
                {0, "четыре"},
                {1, "раз"},
                {1, "два"},
                {1, "пять"}
            };

        SUBCASE("при последовательной сортировке")
        {
            auto numbers = initial;
            burst::radix_sort(numbers.begin(), numbers.end(), buffer.begin(),
                [] (const auto & pair)
                {
                    return pair.first;
                });

            CHECK(numbers == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            auto numbers = initial;
            burst::radix_sort(burst::par(2), numbers.begin(), numbers.end(), buffer.begin(),
                [] (const auto & pair)
                {
                    return pair.first;
                });

            CHECK(numbers == expected);
        }
    }

    TEST_CASE("Умеет сортировать знаковые числа")
    {
        const auto initial =
            std::vector<std::int64_t>
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
        std::vector<std::int64_t> buffer(initial.size());

        const auto expected =
            std::vector<std::int64_t>
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

        SUBCASE("при последовательной сортировке")
        {
            auto values = initial;
            burst::radix_sort(values.begin(), values.end(), buffer.begin());

            CHECK(values == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            auto values = initial;
            burst::radix_sort(burst::par(3), values.begin(), values.end(), buffer.begin());

            CHECK(values == expected);
        }
    }

    TEST_CASE("Возможно сортировать некопируемые объекты")
    {
        const auto expected = {-100500, 0, 5, 30, 20152016};
        SUBCASE("при последовательной сортировке")
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

            CHECK(std::equal
            (
                boost::make_indirect_iterator(std::begin(pointers)),
                boost::make_indirect_iterator(std::end(pointers)),
                std::begin(expected)
            ));
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::unique_ptr<std::int64_t>> pointers;
            pointers.emplace_back(std::make_unique<std::int64_t>(30));
            pointers.emplace_back(std::make_unique<std::int64_t>(5));
            pointers.emplace_back(std::make_unique<std::int64_t>(-100500));
            pointers.emplace_back(std::make_unique<std::int64_t>(20152016));
            pointers.emplace_back(std::make_unique<std::int64_t>(0));

            std::vector<std::unique_ptr<std::int64_t>> buffer(pointers.size());
            burst::radix_sort(burst::par(2), pointers.begin(), pointers.end(), buffer.begin(),
                [] (const auto & p)
                {
                    return *p;
                });

            CHECK(std::equal
            (
                boost::make_indirect_iterator(std::begin(pointers)),
                boost::make_indirect_iterator(std::end(pointers)),
                std::begin(expected)
            ));
        }
    }

    TEST_CASE("Возможно сортировать некопируемые одноразрядные объекты")
    {
        const auto expected = {-0xf, 0, 0x5, 0x30, 0x7f};
        SUBCASE("при последовательной сортировке")
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

            CHECK(std::equal
            (
                boost::make_indirect_iterator(std::begin(pointers)),
                boost::make_indirect_iterator(std::end(pointers)),
                std::begin(expected)
            ));
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::unique_ptr<std::int8_t>> pointers;
            pointers.emplace_back(std::make_unique<std::int8_t>(0x30));
            pointers.emplace_back(std::make_unique<std::int8_t>(0x5));
            pointers.emplace_back(std::make_unique<std::int8_t>(-0xf));
            pointers.emplace_back(std::make_unique<std::int8_t>(0x7f));
            pointers.emplace_back(std::make_unique<std::int8_t>(0));

            std::vector<std::unique_ptr<std::int8_t>> buffer(pointers.size());
            burst::radix_sort(burst::par(3), pointers.begin(), pointers.end(), buffer.begin(),
                [] (const auto & p)
                {
                    return *p;
                });

            CHECK(std::equal
            (
                boost::make_indirect_iterator(std::begin(pointers)),
                boost::make_indirect_iterator(std::end(pointers)),
                std::begin(expected)
            ));
        }
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

        SUBCASE("при последовательной сортировке")
        {
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
        SUBCASE("при параллельной сортировке")
        {
            auto objects =
                std::vector<object_type>
                {
                    object_type(100500),
                    object_type(-200),
                    object_type(0)
                };
            std::vector<object_type> buffer(objects.size());
            burst::radix_sort(burst::par(2), objects, buffer.begin(), [] (auto && n) {return n.n;});
        }
    }

    TEST_CASE("Допускает на вход диапазоны")
    {
        const auto initial = std::vector<std::uint32_t>{100500, 42, 99999, 1000, 0};
        std::vector<std::uint32_t> buffer(initial.size());

        const auto  expected = std::vector<std::uint32_t>{0, 42, 1000, 99999, 100500};

        SUBCASE("при последовательной сортировке")
        {
            auto numbers = initial;
            burst::radix_sort(numbers, buffer.begin());

            CHECK(numbers == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            auto numbers = initial;
            burst::radix_sort(burst::par(4), numbers, buffer.begin());

            CHECK(numbers == expected);
        }
    }

    TEST_CASE("Поразрядная сортировка может быть распараллелена")
    {
        std::vector<std::uint32_t> numbers{100500, 42, 99999, 1000, 0};

        std::vector<std::uint32_t> buffer(numbers.size());
        burst::radix_sort(burst::par(2), numbers.begin(), numbers.end(), buffer.begin());

        std::vector<std::uint32_t> expected{0, 42, 1000, 99999, 100500};
        CHECK(numbers == expected);
    }

    TEST_CASE_TEMPLATE("Непосредственно сортировка выполняется в пуле потоков, в вызывающем потоке "
        "обращений к разрядам нет", integer_type, std::int8_t, std::int16_t, std::int32_t)
    {
        auto numbers = utility::random_vector<integer_type>(1000, -100, 100);

        std::unordered_set<std::thread::id> thread_ids;
        std::mutex m;

        std::vector<integer_type> buffer(numbers.size());
        constexpr auto thread_count = 3ul;
        burst::radix_sort(burst::par(thread_count), numbers.begin(), numbers.end(), buffer.begin(),
            [& thread_ids, & m] (auto x)
            {
                std::unique_lock<std::mutex> l(m);
                thread_ids.insert(std::this_thread::get_id());
                return x;
            });

        CHECK(thread_ids.size() <= thread_count);
        CHECK(thread_ids.find(std::this_thread::get_id()) == thread_ids.end());
    }

    TEST_CASE("Если заявлено меньше двух потоков, то алгоритм откатывается на однопоточный вариант")
    {
        std::vector<std::uint32_t> numbers{0, 5, 1, 6, 2, 100500};

        std::unordered_set<std::thread::id> thread_ids;
        std::mutex m;

        std::vector<std::uint32_t> buffer(numbers.size());
        burst::radix_sort(burst::par(1), numbers.begin(), numbers.end(), buffer.begin(),
            [& thread_ids, & m] (auto x)
            {
                std::unique_lock<std::mutex> l(m);
                thread_ids.insert(std::this_thread::get_id());
                return x;
            });

        CHECK(thread_ids.size() == 1);
        CHECK(thread_ids.find(std::this_thread::get_id()) != thread_ids.end());
    }
}
