#include <burst/container/dynamic_tuple.hpp>

#include <boost/program_options.hpp>

#include <chrono>
#include <iostream>

void test (std::size_t size, std::size_t attempt_count)
{
    using namespace std::chrono;
    auto total_time = steady_clock::duration{0};

    for (std::size_t attempt = 0; attempt < attempt_count; ++attempt)
    {
        auto attempt_start_time = steady_clock::now();

        burst::dynamic_tuple t;
        for (std::size_t iteration = 0; iteration < size; ++iteration)
        {
            t.push_back('a');
        }

        auto attempt_time = steady_clock::now() - attempt_start_time;

        total_time += attempt_time;
    }

    std::cout << "Время: " << ' ' << duration_cast<duration<double>>(total_time).count() << std::endl;
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
