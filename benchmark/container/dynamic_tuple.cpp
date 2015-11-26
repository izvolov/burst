#include <iostream>

#include <boost/program_options.hpp>

#include <burst/container/dynamic_tuple.hpp>

void test (std::size_t size, std::size_t attempt_count)
{
    clock_t total_time = 0;
    for (std::size_t attempt = 0; attempt < attempt_count; ++attempt)
    {
        clock_t attempt_time = clock();

        burst::dynamic_tuple t;
        for (std::size_t iteration = 0; iteration < size; ++iteration)
        {
            t.push_back('a');
        }

        attempt_time = clock() - attempt_time;

        total_time += attempt_time;
    }

    std::cout << "Время: " << static_cast<double>(total_time) / CLOCKS_PER_SEC << std::endl;
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("size", bpo::value<std::size_t>()->default_value(1000))
        ("attempts", bpo::value<std::size_t>()->default_value(1000));

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
            std::size_t size = vm["size"].as<std::size_t>();
            std::size_t attempts = vm["attempts"].as<std::size_t>();

            test(size, attempts);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
