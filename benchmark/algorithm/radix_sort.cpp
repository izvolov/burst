#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

#include <burst/algorithm/sorting/radix_sort.hpp>

template <typename Container>
void read (std::istream & stream, Container & values)
{
    for (std::int64_t n; stream >> n; /* пусто */)
    {
        values.push_back(static_cast<typename Container::value_type>(n));
    }
}

template <typename Container>
void test_radix_sort (const Container & numbers, std::size_t attempts)
{
    clock_t total_time = 0;

    for (std::size_t attempt = 0; attempt < attempts; ++attempt)
    {
        auto unsorted = numbers;

        clock_t attempt_time = clock();
        burst::radix_sort(unsorted.begin(), unsorted.end());
        attempt_time = clock() - attempt_time;
        total_time += attempt_time;
    }

    std::cout << "Поразрядная:" << std::endl;
    std::cout << "\tОбщее время: " << static_cast<double>(total_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << "\tСреднее время: " << static_cast<double>(total_time) / static_cast<double>(attempts) / CLOCKS_PER_SEC << std::endl;
    std::cout << std::endl;
}

template <typename Container>
void test_std_sort (const Container & numbers, std::size_t attempts)
{
    clock_t total_time = 0;

    for (std::size_t attempt = 0; attempt < attempts; ++attempt)
    {
        auto unsorted = numbers;

        clock_t attempt_time = clock();
        std::sort(unsorted.begin(), unsorted.end());
        attempt_time = clock() - attempt_time;
        total_time += attempt_time;
    }

    std::cout << "std::sort:" << std::endl;
    std::cout << "\tОбщее время: " << static_cast<double>(total_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << "\tСреднее время: " << static_cast<double>(total_time) / static_cast<double>(attempts) / CLOCKS_PER_SEC << std::endl;
    std::cout << std::endl;
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
            test_radix_sort(numbers, attempts);
            test_std_sort(numbers, attempts);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
