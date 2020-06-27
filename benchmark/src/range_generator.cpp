#include <utility/io/generate.hpp>

#include <boost/program_options.hpp>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("count", bpo::value<std::size_t>(),
            "Количество генерируемых наборов")
        ("length", bpo::value<std::size_t>(),
            "Длина одного набора")
        ("min", bpo::value<std::int64_t>()->default_value(std::numeric_limits<std::int64_t>::min()),
            "Минимальное значение элемента набора")
        ("max", bpo::value<std::int64_t>()->default_value(std::numeric_limits<std::int64_t>::max()),
            "Максимальное значение элемента набора")
        ("seed", bpo::value<bool>()->implicit_value(true)->default_value(false),
            "Выбрать произвольную точку начала последовательности псевдослучайных чисел")
        ("sort", bpo::value<bool>()->implicit_value(true)->default_value(false),
            "Породить упорядоченный набор")
        ("descending", bpo::value<bool>()->implicit_value(true)->default_value(false),
            "Упорядочить набор по убыванию");

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

            bool seed = vm["seed"].as<bool>();
            bool sort = vm["sort"].as<bool>();
            bool descending = vm["descending"].as<bool>();

            utility::generate(std::cout, range_count, range_length, min, max, seed, sort, descending);
        }
    }
    catch (std::exception &)
    {
        std::cout << description << std::endl;
    }
}
