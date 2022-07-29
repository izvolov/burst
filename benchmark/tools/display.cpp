#include <burst/functional/trivial_read.hpp>
#include <burst/range/binary_istream_range.hpp>

#include <boost/program_options.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <iterator>

void display (std::istream & in, std::ostream & out)
{
    auto size = std::size_t{};
    while (burst::trivial_read(in, size))
    {
        auto input = burst::make_binary_istream_range<std::int64_t>(in);

        using std::begin;
        std::copy_n(begin(input), size, std::ostream_iterator<std::int64_t>(out, " "));
        out << std::endl;
    }
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "На вход ожидаются диапазоны в формате\nN X1 ... XN\nM Y1 ... YM\n...");

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
            display(std::cin, std::cout);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
