#include <algorithm>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/range/algorithm/for_each.hpp>

void generate (std::size_t range_count, std::size_t range_length, std::int64_t min, std::int64_t max, bool sort, bool descending)
{
    std::default_random_engine engine;
    std::uniform_int_distribution<std::int64_t> uniform(min, max);

    std::vector<std::int64_t> range(range_length);
    for (std::size_t i = 0; i < range_count; ++i)
    {
        std::generate(range.begin(), range.end(), [&] () { return uniform(engine); });
        if (sort)
        {
            if (descending)
            {
                std::sort(range.begin(), range.end(), std::greater<>());
            }
            else
            {
                std::sort(range.begin(), range.end());
            }
        }

        boost::for_each(range, [] (std::int64_t value) { std::cout << value << ' '; });
        std::cout << "\n";
    }
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("count", bpo::value<std::size_t>(), "Количество генерируемых наборов")
        ("length", bpo::value<std::size_t>(), "Длина одного набора")
        ("min", bpo::value<std::int64_t>()->default_value(std::numeric_limits<std::int64_t>::min()), "Минимальное значение элемента набора")
        ("max", bpo::value<std::int64_t>()->default_value(std::numeric_limits<std::int64_t>::max()), "Максимальное значение элемента набора")
        ("sort", bpo::value<bool>()->implicit_value(true)->default_value(false), "Породить упорядоченный набор")
        ("descending", bpo::value<bool>()->implicit_value(true)->default_value(false), "Упорядочить набор по убыванию");

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
            std::size_t range_count = vm["count"].as<std::size_t>();
            std::size_t range_length = vm["length"].as<std::size_t>();
            std::int64_t min = vm["min"].as<std::int64_t>();
            std::int64_t max = vm["max"].as<std::int64_t>();

            bool sort = vm["sort"].as<bool>();
            bool descending = vm["descending"].as<bool>();

            generate(range_count, range_length, min, max, sort, descending);
        }
    }
    catch (std::exception &)
    {
        std::cout << description << std::endl;
    }
}
