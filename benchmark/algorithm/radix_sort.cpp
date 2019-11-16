#include <burst/algorithm/radix_sort.hpp>
#include <burst/functional/identity.hpp>
#include <io.hpp>

#include <boost/program_options.hpp>
#include <boost/sort/spreadsort/integer_sort.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

template <typename Sort, typename Container, typename UnaryFunction>
void
    test_sort
    (
        const std::string & name,
        Sort sort,
        const Container & numbers,
        std::size_t attempts,
        UnaryFunction prepare
    )
{
    using namespace std::chrono;
    auto total_time = steady_clock::duration{0};

    for (std::size_t attempt = 0; attempt < attempts; ++attempt)
    {
        auto unsorted = prepare(numbers);

        auto attempt_start_time = steady_clock::now();
        sort(unsorted.begin(), unsorted.end());
        auto attempt_time = steady_clock::now() - attempt_start_time;
        total_time += attempt_time;
    }

    std::cout << name << ' ' << duration_cast<duration<double>>(total_time).count() << std::endl;
}

template <typename Integer, typename UnaryFunction>
void test_all (std::size_t attempts, UnaryFunction prepare)
{
    std::vector<Integer> numbers;
    read(std::cin, numbers);

    std::vector<Integer> buffer(numbers.size());

    auto radix_sort =
        [& buffer] (auto && ... args)
        {
            return burst::radix_sort(std::forward<decltype(args)>(args)..., buffer.begin());
        };
    test_sort("burst::radix_sort", radix_sort, numbers, attempts, prepare);

    auto std_sort =
        [] (auto && ... args)
        {
            return std::sort(std::forward<decltype(args)>(args)...);
        };
    test_sort("std::sort", std_sort, numbers, attempts, prepare);

    auto std_stable_sort =
        [] (auto && ... args)
        {
            return std::stable_sort(std::forward<decltype(args)>(args)...);
        };
    test_sort("std::stable_sort", std_stable_sort, numbers, attempts, prepare);

    auto boost_int_sort =
        [] (auto && ... args)
        {
            return boost::sort::spreadsort::integer_sort(std::forward<decltype(args)>(args)...);
        };
    test_sort("boost::integer_sort", boost_int_sort, numbers, attempts, prepare);
}

using test_call_type = std::function<void (std::size_t)>;

template <typename Integer>
struct shuffle_fn
{
    template <typename Container>
    auto operator () (Container c)
    {
        std::shuffle(c.begin(), c.end(), std::mt19937((*rd)()));
        return c;
    }

    std::shared_ptr<std::random_device> rd = std::make_shared<std::random_device>();
};

template <typename BinaryPredicate>
struct sort_fn
{
    template <typename Container>
    auto operator () (Container c) const
    {
        std::sort(c.begin(), c.end(), compare);
        return c;
    }

    BinaryPredicate compare = BinaryPredicate{};
};

// Сортирует диапазон и ставит его максимальное значение на случайную позицию.
struct outlier_fn
{
    template <typename Container>
    auto operator () (Container c)
    {
        if (c.size() > 1)
        {
            std::sort(c.begin(), c.end());

            std::default_random_engine engine((*rd)());
            const auto max_outlier_index = std::distance(c.begin(), c.end()) - 2;
            using difference_type = typename Container::difference_type;
            auto uniform = std::uniform_int_distribution<difference_type>(0, max_outlier_index);
            auto outlier_index = uniform(engine);
            std::rotate(c.begin() + outlier_index, c.end() - 1, c.end());
        }
        return c;
    }

    std::shared_ptr<std::random_device> rd = std::make_shared<std::random_device>();
};

template <typename Integer>
test_call_type dispatch_preparation (const std::string & prepare_type)
{
    static const auto test_calls =
        std::unordered_map<std::string, test_call_type>
        {
            {
                "shuffle",
                [shuffle = shuffle_fn<Integer>{}] (std::size_t attempts)
                {
                    return test_all<Integer>(attempts, shuffle);
                }
            },
            {
                "ascending",
                [sort = sort_fn<std::less<>>{}] (std::size_t attempts)
                {
                    return test_all<Integer>(attempts, sort);
                }
            },
            {
                "descending",
                [sort = sort_fn<std::greater<>>{}] (std::size_t attempts)
                {
                    return test_all<Integer>(attempts, sort);
                }
            },
            {
                "outlier",
                [swap = outlier_fn{}] (std::size_t attempts)
                {
                    return test_all<Integer>(attempts, swap);
                }
            }
        };

    auto call = test_calls.find(prepare_type);
    if (call != test_calls.end())
    {
        return call->second;
    }
    else
    {
        throw boost::program_options::error(u8"Неверный тип подготовки массива: " + prepare_type);
    }
}

using dispatch_preparation_call_type = std::function<test_call_type (const std::string &)>;

dispatch_preparation_call_type dispatch_integer (const std::string & integer_type)
{
    static const auto dispatch_prepare_calls =
        std::unordered_map<std::string, dispatch_preparation_call_type>
        {
            {"uint8", &dispatch_preparation<std::uint8_t>},
            {"uint16", &dispatch_preparation<std::uint16_t>},
            {"uint32", &dispatch_preparation<std::uint32_t>},
            {"uint64", &dispatch_preparation<std::uint64_t>},
            {"int8", &dispatch_preparation<std::int8_t>},
            {"int16", &dispatch_preparation<std::int16_t>},
            {"int32", &dispatch_preparation<std::int32_t>},
            {"int64", &dispatch_preparation<std::int64_t>}
        };

    auto call = dispatch_prepare_calls.find(integer_type);
    if (call != dispatch_prepare_calls.end())
    {
        return call->second;
    }
    else
    {
        auto error_message = u8"Неверная разрядность сортируемых чисел: " + integer_type;
        throw boost::program_options::error(error_message);
    }
}

test_call_type dispatch_call (const std::string & integer_type, const std::string & prepare_type)
{
    return dispatch_integer(integer_type)(prepare_type);
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("attempts", bpo::value<std::size_t>()->default_value(1000),
            "Количество испытаний")
        ("integer", bpo::value<std::string>()->default_value("uint32"),
            "Тип сортируемых чисел.\n"
            "Допустимые значения: uint8, uint16, uint32, uint64, int8, int16, int32, int64")
        ("prepare", bpo::value<std::string>()->required(),
            "Тип подготовки массива перед каждым испытанием.\n"
            "Допустимые значения: shuffle, ascending, descending, outlier");

    try
    {
        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, description), vm);
        bpo::notify(vm);

        if (vm.count("help"))
        {
            std::cout << description << std::endl;
        }
        else
        {
            std::size_t attempts = vm["attempts"].as<std::size_t>();
            auto integer_type = vm["integer"].as<std::string>();
            auto prepare_type = vm["prepare"].as<std::string>();

            auto test = dispatch_call(integer_type, prepare_type);
            test(attempts);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
