#include <burst/integer/divceil.hpp>
#include <io.hpp>

#include <boost/program_options.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

template <typename Container, typename Integer>
void test_divceil (const Container & values, Integer divisor, std::size_t attempt_count)
{
    using namespace std::chrono;
    using value_type = typename Container::value_type;

    const auto start_time = steady_clock::now();

    auto total = value_type{0};
    for (std::size_t iteration = 0; iteration < attempt_count; ++iteration)
    {
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

template <typename Integer1, typename Integer2>
void test_all (std::int64_t divisor, std::size_t attempt_count)
{
    std::vector<Integer1> values;
    read(std::cin, values);

    test_divceil(values, static_cast<Integer2>(divisor), attempt_count);
}

using test_call_type = void (*) (std::int64_t, std::size_t);
template <typename Integer>
test_call_type dispatch_divisor (const std::string & divisor_type)
{
    static const auto test_calls =
        std::unordered_map<std::string, test_call_type>
        {
            {"uint8", &test_all<Integer, std::uint8_t>},
            {"uint16", &test_all<Integer, std::uint16_t>},
            {"uint32", &test_all<Integer, std::uint32_t>},
            {"uint64", &test_all<Integer, std::uint64_t>},
            {"int8", &test_all<Integer, std::int8_t>},
            {"int16", &test_all<Integer, std::int16_t>},
            {"int32", &test_all<Integer, std::int32_t>},
            {"int64", &test_all<Integer, std::int64_t>}
        };

    auto call = test_calls.find(divisor_type);
    if (call != test_calls.end())
    {
        return call->second;
    }
    else
    {
        throw boost::program_options::error(u8"Неверная разрядность делителя: " + divisor_type);
    }
}

test_call_type get_call (const std::string & integer_type, const std::string & divisor_type)
{
    using divisor_dispatcher = test_call_type (*) (const std::string &);
    static const auto dispatchers =
        std::unordered_map<std::string, divisor_dispatcher>
        {
            {"uint8", &dispatch_divisor<std::uint8_t>},
            {"uint16", &dispatch_divisor<std::uint16_t>},
            {"uint32", &dispatch_divisor<std::uint32_t>},
            {"uint64", &dispatch_divisor<std::uint64_t>},
            {"int8", &dispatch_divisor<std::int8_t>},
            {"int16", &dispatch_divisor<std::int16_t>},
            {"int32", &dispatch_divisor<std::int32_t>},
            {"int64", &dispatch_divisor<std::int64_t>}
        };

    auto dispatcher = dispatchers.find(integer_type);
    if (dispatcher != dispatchers.end())
    {
        return dispatcher->second(divisor_type);
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
        ("help,h", "Подсказка.")
        ("attempts", bpo::value<std::size_t>()->default_value(1000),
            "Количество испытаний.")
        ("integer", bpo::value<std::string>()->default_value("int32"),
            "Тип сортируемых чисел.\n"
            "Допустимые значения: uint8, uint16, uint32, uint64, int8, int16, int32, int64")
        ("divisor", bpo::value<boost::tuple<std::string, std::int64_t>>()->default_value(boost::make_tuple("int32", 100)),
            "Тип делителя.\n"
            "Допустимые значения: uint8, uint16, uint32, uint64, int8, int16, int32, int64");

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
            auto integer_type = vm["integer"].as<std::string>();
            auto divisor = vm["divisor"].as<boost::tuple<std::string, std::int64_t>>();

            auto test = get_call(integer_type, boost::get<0>(divisor));
            test(boost::get<1>(divisor), attempts);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
