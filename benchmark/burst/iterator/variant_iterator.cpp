#include <utility/io/generate.hpp>

#include <burst/iterator/variant_iterator.hpp>

#include <boost/program_options.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <list>
#include <random>
#include <vector>

auto random_numbers (std::size_t size)
{
    std::vector<std::int64_t> numbers(size);

    std::default_random_engine generator;
    std::uniform_int_distribution<std::int64_t> uniform;
    std::generate_n(numbers.begin(), size,
        [& uniform, & generator] {return uniform(generator);});

    return numbers;
}

template <typename Iterator>
void test (const std::string & name, Iterator first, Iterator last)
{
    using namespace std::chrono;
    const auto start_time = steady_clock::now();

    auto sum = std::int64_t{0};
    while (first != last)
    {
        sum += *first;
        ++first;
    }

    std::clog << sum << std::endl;
    const auto total_time = steady_clock::now() - start_time;
    std::cout << name << ' ' << duration_cast<duration<double>>(total_time).count() << std::endl;
}

void test (std::size_t size)
{
    auto numbers = random_numbers(size);

    test("raw", numbers.begin(), numbers.end());

    using variant_iterator =
        burst::variant_iterator
        <
            std::vector<std::int64_t>::iterator,
            std::list<std::int64_t>::iterator,
            std::deque<std::int64_t>::iterator,
            std::int64_t *
        >;
    test("variant", variant_iterator(numbers.begin()), variant_iterator(numbers.end()));
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка.")
        ("size", bpo::value<std::size_t>()->default_value(1000000),
            "Размер массива, на котором будут проходить проверки");

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
            auto size = vm["size"].as<std::size_t>();

            test(size);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
