#include <burst/algorithm/galloping_lower_bound.hpp>
#include <io.hpp>

#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

template <typename Container, typename Function>
void test (const Container & numbers, std::size_t attempts, Function lower_bound, const std::string & name)
{
    clock_t total_time = 0;

    for (std::size_t attempt = 0; attempt < attempts; ++attempt)
    {
        using difference_type = typename Container::difference_type;
        auto search_start = numbers.begin() + random() % static_cast<difference_type>(numbers.size() - 1);
        auto search_position = static_cast<std::size_t>(std::sqrt(std::distance(search_start, numbers.end())));
        auto search_element = search_start + random() % static_cast<difference_type>(search_position);
        auto number = *search_element;

        clock_t attempt_time = clock();
        lower_bound(search_start, numbers.end(), number);
        attempt_time = clock() - attempt_time;
        total_time += attempt_time;
    }

    std::cout << name << ":" << std::endl;
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
            std::vector<std::int64_t> numbers;
            read(std::cin, numbers);

            std::size_t attempts = vm["attempts"].as<std::size_t>();
            test(numbers, attempts, &burst::galloping_lower_bound<std::vector<std::int64_t>::const_iterator, std::int64_t>, "gallop");
            test(numbers, attempts, &std::lower_bound<std::vector<std::int64_t>::const_iterator, std::int64_t>, "std::lower_bound");
            test(numbers, attempts, &std::find<std::vector<std::int64_t>::const_iterator, std::int64_t>, "std::find");
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
