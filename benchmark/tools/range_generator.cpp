#include <utility/io/generate_many.hpp>
#include <utility/io/generate_many_sorted.hpp>

#include <burst/iterator/binary_ostream_iterator.hpp>

#include <boost/program_options.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <random>

void
    do_generate
    (
        std::ostream & stream,
        std::size_t block_size,
        std::size_t range_count,
        std::size_t range_length,
        std::int64_t min,
        std::int64_t max,
        bool seed,
        bool sort,
        bool descending
    )
{
    const auto time = std::chrono::system_clock::now().time_since_epoch().count();
    const auto seed_value = seed ? static_cast<std::default_random_engine::result_type>(time) : 0;
    std::default_random_engine generator(seed_value);

    auto result = burst::make_binary_ostream_iterator(stream);
    if (sort)
    {
        std::vector<std::int64_t> buffer(block_size);
        if (descending)
        {
            utility::generate_many_sorted
            (
                generator,
                block_size,
                range_count,
                range_length,
                min,
                max,
                std::greater<>{},
                result,
                buffer.begin()
            );
        }
        else
        {
            utility::generate_many_sorted
            (
                generator,
                block_size,
                range_count,
                range_length,
                min,
                max,
                std::less<>{},
                result,
                buffer.begin()
            );
        }
    }
    else
    {
        utility::generate_many(generator, range_count, range_length, min, max, result);
    }
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("block", bpo::value<std::size_t>()->default_value(1 << 20),
            "Размер используемой памяти в элементах")
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
            std::size_t block_size = vm["block"].as<std::size_t>();
            std::size_t range_count = vm["count"].as<std::size_t>();
            std::size_t range_length = vm["length"].as<std::size_t>();
            std::int64_t min = vm["min"].as<std::int64_t>();
            std::int64_t max = vm["max"].as<std::int64_t>();

            bool seed = vm["seed"].as<bool>();
            bool sort = vm["sort"].as<bool>();
            bool descending = vm["descending"].as<bool>();

            do_generate(std::cout, block_size, range_count, range_length, min, max, seed, sort, descending);
        }
    }
    catch (std::exception &)
    {
        std::cout << description << std::endl;
    }
}
