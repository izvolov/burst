#include <algorithm>
#include <iostream>
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
    clock_t total_time = 0;

    for (std::size_t attempt = 0; attempt < attempts; ++attempt)
    {
        auto unsorted = numbers;

        clock_t attempt_time = clock();
        sort(unsorted.begin(), unsorted.end());
        attempt_time = clock() - attempt_time;
        total_time += attempt_time;
    }

    std::cout << name << ": " << static_cast<double>(total_time) / CLOCKS_PER_SEC << std::endl;
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("attempts", bpo::value<std::size_t>()->default_value(1000), "Количество испытаний");

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
            std::vector<std::uint32_t> numbers;
            read(std::cin, numbers);

            std::size_t attempts = vm["attempts"].as<std::size_t>();

            auto radix_sort = [] (auto && ... args) { return burst::radix_sort(std::forward<decltype(args)>(args)...); };
            test_sort("burst::radix_sort", radix_sort, numbers, attempts);

            auto std_sort = [] (auto && ... args) { return std::sort(std::forward<decltype(args)>(args)...); };
            test_sort("std::sort", std_sort, numbers, attempts);

            auto boost_int_sort = [] (auto && ... args) { return boost::sort::spreadsort::integer_sort(std::forward<decltype(args)>(args)...); };
            test_sort("boost::integer_sort", boost_int_sort, numbers, attempts);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
