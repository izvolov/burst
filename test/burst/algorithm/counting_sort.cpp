#include <utility/io/vector.hpp>
#include <utility/random_vector.hpp>

#include <burst/algorithm/counting_sort.hpp>
#include <burst/execution/parallel_policy.hpp>

#include <doctest/doctest.h>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/range/adaptor/indirected.hpp>
#include <boost/range/iterator_range.hpp>

#include <forward_list>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>

TEST_SUITE("counting_sort")
{
    TEST_CASE("Сортировка отсортированного диапазона не изменяет его")
    {
        std::vector<std::uint8_t> values{0, 1, 2, 3, 4};

        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint8_t> sorted_values(values.size());
            burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

            CHECK(std::is_sorted(sorted_values.begin(), sorted_values.end()));
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> sorted_values(values.size());
            burst::counting_sort_copy
            (
                burst::par(4),
                values.begin(),
                values.end(),
                sorted_values.begin()
            );

            CHECK(std::is_sorted(sorted_values.begin(), sorted_values.end()));
        }
    }

    TEST_CASE("Результат сортировки убывающего диапазона — возрастающий диапазон")
    {
        std::vector<std::size_t> values{0x0104, 0x0203, 0x0302, 0x0401};

        std::vector<std::size_t> expected{0x0401, 0x0302, 0x0203, 0x0104};
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::size_t> sorted_values(values.size());
            burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin(),
                [] (const std::size_t & integer) -> std::uint8_t
                {
                    return integer & 0xff;
                });

            CHECK(sorted_values == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::size_t> sorted_values(values.size());
            burst::counting_sort_copy
            (
                burst::par(2),
                values.begin(),
                values.end(),
                sorted_values.begin(),
                [] (const std::size_t & integer) -> std::uint8_t
                {
                    return integer & 0xff;
                }
            );

            CHECK(sorted_values == expected);
        }
    }

    TEST_CASE("Умеет сортировать побитово")
    {
        std::vector<std::uint8_t> numbers{0, 5, 3, 7, 1, 2, 4, 6};

        // Порядок внутри чётных и нечётных групп сохраняется, потому что сортировка устойчива.
        std::vector<std::uint8_t> even_goes_first{0, 2, 4, 6, 5, 3, 7, 1};

        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint8_t> sorted_numbers(numbers.size());
            burst::counting_sort_copy(numbers.begin(), numbers.end(), sorted_numbers.begin(),
                [] (const std::uint8_t & number) -> bool
                {
                    return number & 0x01;
                });

            CHECK(sorted_numbers == even_goes_first);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> sorted_numbers(numbers.size());
            burst::counting_sort_copy
            (
                burst::par(4),
                numbers.begin(),
                numbers.end(),
                sorted_numbers.begin(),
                [] (const std::uint8_t & number) -> bool
                {
                    return number & 0x01;
                }
            );

            CHECK(sorted_numbers == even_goes_first);
        }
    }

    TEST_CASE("Сортировка хаоса приводит к порядку")
    {
        std::vector<std::uint8_t> values{0x12, 0xfd, 0x00, 0x15, 0x66};

        std::vector<std::uint8_t> expected{0x00, 0x12, 0x15, 0x66, 0xfd};
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint8_t> sorted_values(values.size());
            burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

            CHECK(sorted_values == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> sorted_values(values.size());
            burst::counting_sort_copy
            (
                burst::par(3),
                values.begin(),
                values.end(),
                sorted_values.begin()
            );

            CHECK(sorted_values == expected);
        }
    }

    TEST_CASE("Отрицательные значения предшествуют неотрицательным в результате сортировки")
    {
        std::vector<std::int8_t> values{0, -1, 1, -2, 2};

        std::vector<std::int8_t> expected{-2, -1, 0, 1, 2};
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::int8_t> sorted_values(values.size());
            burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

            CHECK(sorted_values == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::int8_t> sorted_values(values.size());
            burst::counting_sort_copy
            (
                burst::par(2),
                values.begin(),
                values.end(),
                sorted_values.begin()
            );

            CHECK(sorted_values == expected);
        }
    }

    TEST_CASE("Экстремальные значения элементов сортируются корректно")
    {
        std::vector<std::int8_t> values
        {
            0,
            std::numeric_limits<std::int8_t>::min(),
            std::numeric_limits<std::int8_t>::max()
        };

        std::vector<std::int8_t> expected
        {
            std::numeric_limits<std::int8_t>::min(),
            0,
            std::numeric_limits<std::int8_t>::max()
        };
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::int8_t> sorted_values(values.size());
            burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin());

            CHECK(sorted_values == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::int8_t> sorted_values(values.size());
            burst::counting_sort_copy
            (
                burst::par(2),
                values.begin(),
                values.end(),
                sorted_values.begin()
            );

            CHECK(sorted_values == expected);
        }
    }

    TEST_CASE("Алгоритм сортировки устойчив")
    {
        std::vector<std::uint32_t> values
        {
            0x3301,
            0x3300,
            0x2201,
            0x2200,
            0x1101,
            0x1100
        };

        std::vector<std::uint32_t> expected
        {
            0x3300,
            0x2200,
            0x1100,
            0x3301,
            0x2201,
            0x1101
        };
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint32_t> sorted_values(values.size());
            burst::counting_sort_copy(values.begin(), values.end(), sorted_values.begin(),
                [] (const std::uint32_t & integer) -> std::uint8_t
                {
                    return integer & 0xff;
                });

            CHECK(sorted_values == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint32_t> sorted_values(values.size());
            burst::counting_sort_copy
            (
                burst::par(3),
                values.begin(),
                values.end(),
                sorted_values.begin(),
                [] (const std::uint32_t & integer) -> std::uint8_t
                {
                    return integer & 0xff;
                }
            );

            CHECK(sorted_values == expected);
        }
    }

    TEST_CASE("Возвращает итератор после последнего отсортированного элемента в результирующем "
        "диапазоне")
    {
        std::vector<std::uint8_t> initial{5, 4, 3, 2, 1};

        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint8_t> sorted(initial.size() * 2);
            auto sorted_end =
                burst::counting_sort_copy(initial.begin(), initial.end(), sorted.begin());

            CHECK(sorted_end == sorted.begin() + std::distance(initial.begin(), initial.end()));
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> sorted(initial.size() * 2);
            auto sorted_end =
                burst::counting_sort_copy
                (
                    burst::par(2),
                    initial.begin(),
                    initial.end(),
                    sorted.begin()
                );

            CHECK(sorted_end == sorted.begin() + std::distance(initial.begin(), initial.end()));
        }
    }

    TEST_CASE("Исходный диапазон не изменяется")
    {
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint8_t> numbers{5, 4, 3, 2, 1};
            const auto numbers_copy = numbers;

            std::vector<std::uint8_t> sorted_numbers(numbers.size());
            burst::counting_sort_copy(numbers.begin(), numbers.end(), sorted_numbers.begin());

            CHECK(numbers == numbers_copy);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> numbers{5, 4, 3, 2, 1};
            const auto numbers_copy = numbers;

            std::vector<std::uint8_t> sorted_numbers(numbers.size());
            burst::counting_sort_copy
            (
                burst::par(3),
                numbers.begin(),
                numbers.end(),
                sorted_numbers.begin()
            );

            CHECK(numbers == numbers_copy);
        }
    }

    TEST_CASE("Может сортировать некопируемые элементы")
    {
        std::vector<std::unique_ptr<std::int8_t>> unsorted;
        unsorted.emplace_back(std::make_unique<std::int8_t>(3));
        unsorted.emplace_back(std::make_unique<std::int8_t>(5));
        unsorted.emplace_back(std::make_unique<std::int8_t>(-1));
        unsorted.emplace_back(std::make_unique<std::int8_t>(20));
        unsorted.emplace_back(std::make_unique<std::int8_t>(0));

        auto expected = {-1, 0, 3, 5, 20};
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::unique_ptr<std::int8_t>> sorted(unsorted.size());
            burst::counting_sort_move(unsorted.begin(), unsorted.end(), sorted.begin(),
                [] (const auto & p)
                {
                    return *p;
                });

            CHECK(boost::algorithm::all_of(unsorted, [] (const auto & p) { return p == nullptr; }));

            CHECK
            (
                boost::make_iterator_range(boost::make_indirect_iterator(std::begin(sorted)), boost::make_indirect_iterator(std::end(sorted))) ==
                expected
            );
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::unique_ptr<std::int8_t>> sorted(unsorted.size());
            burst::counting_sort_move
            (
                burst::par(4),
                unsorted.begin(),
                unsorted.end(),
                sorted.begin(),
                [] (const auto & p) {return *p;}
            );

            CHECK(boost::algorithm::all_of(unsorted, [] (const auto & p) { return p == nullptr; }));

            CHECK
            (
                boost::make_iterator_range(boost::make_indirect_iterator(std::begin(sorted)), boost::make_indirect_iterator(std::end(sorted))) ==
                expected
            );
        }
    }

    TEST_CASE("Для сортировки достаточно однонаправленного итератора")
    {
        auto unsorted = std::vector<std::int8_t>{4, 3, 2, 1};
        auto forward_list = std::forward_list<std::int8_t>(unsorted.begin(), unsorted.end());

        std::vector<std::int8_t> sorted(unsorted.size(), 0);
        burst::counting_sort_copy(forward_list.begin(), forward_list.end(), sorted.begin());

        auto expected = std::vector<std::int8_t>{1, 2, 3, 4};
        CHECK(sorted == expected);
    }

    TEST_CASE("Копирующий вариант допускает диапазоны на вход")
    {
        std::vector<std::uint8_t> values{5, 4, 3, 2, 1};

        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::uint8_t> sorted_values(values.size());
            burst::counting_sort_copy(values, sorted_values.begin());

            std::vector<std::uint8_t> expected{1, 2, 3, 4, 5};
            CHECK(sorted_values == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::uint8_t> sorted_values(values.size());
            burst::counting_sort_copy(burst::par(2), values, sorted_values.begin());

            std::vector<std::uint8_t> expected{1, 2, 3, 4, 5};
            CHECK(sorted_values == expected);
        }
    }

    TEST_CASE("Переносящий вариант допускает диапазоны на вход")
    {
        std::vector<std::unique_ptr<std::int8_t>> unsorted;
        unsorted.emplace_back(std::make_unique<std::int8_t>(17));
        unsorted.emplace_back(std::make_unique<std::int8_t>(5));
        unsorted.emplace_back(std::make_unique<std::int8_t>(27));
        unsorted.emplace_back(std::make_unique<std::int8_t>(-50));
        unsorted.emplace_back(std::make_unique<std::int8_t>(-1));
        unsorted.emplace_back(std::make_unique<std::int8_t>(127));
        unsorted.emplace_back(std::make_unique<std::int8_t>(0));

        auto expected = {-50, -1, 0, 5, 17, 27, 127};
        SUBCASE("при последовательной сортировке")
        {
            std::vector<std::unique_ptr<std::int8_t>> sorted(unsorted.size());
            burst::counting_sort_move(unsorted, sorted.begin(),
                [] (const auto & p)
                {
                    return *p;
                });

            CHECK((sorted | boost::adaptors::indirected) == expected);
        }
        SUBCASE("при параллельной сортировке")
        {
            std::vector<std::unique_ptr<std::int8_t>> sorted(unsorted.size());
            burst::counting_sort_move(burst::par(4), unsorted, sorted.begin(),
                [] (const auto & p) {
                    return *p;
                });

            CHECK((sorted | boost::adaptors::indirected) == expected);
        }
    }

    TEST_CASE("Сортировка подсчётом может быть распараллелена")
    {
        std::vector<std::int8_t> unsorted{100, -42, 99, 120, 0};

        std::vector<std::int8_t> sorted(unsorted.size());
        burst::counting_sort_copy(burst::par(2), unsorted, sorted.begin());

        std::vector<std::int8_t> expected{-42, 0, 99, 100, 120};
        CHECK(sorted == expected);
    }

    TEST_CASE_TEMPLATE("Непосредственно сортировка выполняется в пуле потоков, в вызывающем потоке "
        "обращений к разрядам нет", integer_type, std::int8_t, std::int16_t)
    {
        auto unsorted = utility::random_vector<integer_type>(1000, 0, 100);

        std::unordered_set<std::thread::id> thread_ids;
        std::mutex m;

        std::vector<integer_type> sorted(unsorted.size());
        constexpr auto thread_count = 3ul;
        burst::counting_sort_copy(burst::par(thread_count), unsorted, sorted.begin(),
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
        std::vector<std::uint8_t> unsorted{0, 5, 1, 6, 2, 111};

        std::unordered_set<std::thread::id> thread_ids;
        std::mutex m;

        std::vector<std::uint8_t> sorted(unsorted.size());
        burst::counting_sort_copy(burst::par(1), unsorted.begin(), unsorted.end(), sorted.begin(),
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
