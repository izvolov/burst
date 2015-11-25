#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/sort/spreadsort/integer_sort.hpp>

#include <burst/algorithm/sorting/radix_sort.hpp>

template <typename Container>
void read (std::istream & stream, Container & values)
{
    for (std::int64_t n; stream >> n; /* пусто */)
    {
        values.push_back(static_cast<typename Container::value_type>(n));
    }
}

template <typename Sort, typename Container>
void test_sort (const std::string & name, Sort sort, const Container & numbers, std::size_t attempts)
{
    using namespace std::chrono;
    auto total_time = steady_clock::duration{0};

    for (std::size_t attempt = 0; attempt < attempts; ++attempt)
    {
        auto unsorted = numbers;

        auto attempt_start_time = steady_clock::now();
        sort(unsorted.begin(), unsorted.end());
        auto attempt_time = steady_clock::now() - attempt_start_time;
        total_time += attempt_time;
    }

    std::cout << name << ' ' << duration_cast<duration<double>>(total_time).count() << std::endl;
}

template <typename Integer>
void test_all (std::size_t attempts)
{
    std::vector<Integer> numbers;
    read(std::cin, numbers);

    std::vector<Integer> buffer(numbers.size());

    auto radix_sort = [& buffer] (auto && ... args) { return burst::radix_sort(std::forward<decltype(args)>(args)..., buffer.begin()); };
    test_sort("burst::radix_sort", radix_sort, numbers, attempts);

    auto std_sort = [] (auto && ... args) { return std::sort(std::forward<decltype(args)>(args)...); };
    test_sort("std::sort", std_sort, numbers, attempts);

    auto boost_int_sort = [] (auto && ... args) { return boost::sort::spreadsort::integer_sort(std::forward<decltype(args)>(args)...); };
    test_sort("boost::integer_sort", boost_int_sort, numbers, attempts);
}

using test_call_type = void (*) (std::size_t);
test_call_type get_call_for_integer (const std::string & integer_type)
{
    static const std::unordered_map<std::string, test_call_type> test_calls
    {
        {"uint8", &test_all<std::uint8_t>},
        {"uint16", &test_all<std::uint16_t>},
        {"uint32", &test_all<std::uint32_t>},
        {"uint64", &test_all<std::uint64_t>},
        {"int8", &test_all<std::int8_t>},
        {"int16", &test_all<std::int16_t>},
        {"int32", &test_all<std::int32_t>},
        {"int64", &test_all<std::int64_t>}
    };

    auto call = test_calls.find(integer_type);
    if (call != test_calls.end())
    {
        return call->second;
    }
    else
    {
        throw boost::program_options::error(u8"Неверная разрядность сортируемых чисел: " + integer_type);
    }
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("attempts", bpo::value<std::size_t>()->default_value(1000), "Количество испытаний")
        ("integer", bpo::value<std::string>()->default_value("uint32"), "Тип сортируемых чисел. Допустимые значения: uint8, uint16, uint32, uint64, int8, int16, int32, int64");

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

            auto test = get_call_for_integer(integer_type);
            test(attempts);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
