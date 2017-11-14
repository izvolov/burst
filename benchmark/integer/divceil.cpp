#include <burst/integer/divceil.hpp>
#include <io.hpp>

#include <boost/program_options.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

template <typename Container>
void test_divceil (const Container & values, std::size_t attempt_count)
{
    using namespace std::chrono;
    using value_type = typename Container::value_type;

    const auto start_time = steady_clock::now();

    auto total = value_type{0};
    for (std::size_t iteration = 0; iteration < attempt_count; ++iteration)
    {
        const auto divisor = 100;
        total +=
            std::accumulate(values.begin(), values.end(), value_type{0},
                [divisor] (auto a, auto b)
                {
                    return a + burst::divceil(b, divisor);
                });
    }

    const auto total_time = steady_clock::now() - start_time;

    std::clog << total << std::endl;
    const auto time = duration_cast<duration<double>>(total_time).count();
    std::cout << "Время: " << time << std::endl;
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка.")
        ("attempts", bpo::value<std::size_t>()->default_value(1000),
            "Количество испытаний.");

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
            auto attempts = vm["attempts"].as<std::size_t>();

            std::vector<std::int32_t> values;
            read(std::cin, values);

            test_divceil(values, attempts);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
