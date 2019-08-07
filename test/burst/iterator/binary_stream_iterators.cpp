#include <burst/iterator/binary_ostream_iterator.hpp>
#include <burst/range/binary_istream_range.hpp>

#include <doctest/doctest.h>

#include <boost/range/algorithm/copy.hpp>

#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

TEST_SUITE("binary_stream_iterators")
{
    TEST_CASE("trivial_objects_are_written_and_read_by_default_io_functions")
    {
        std::vector<int> initial{1, 2, 3};
        static_assert(std::is_trivial<decltype(initial)::value_type>::value, "");

        std::stringstream stream;

        boost::copy(initial, burst::make_binary_ostream_iterator(stream));

        std::vector<int> result;
        boost::copy(burst::make_binary_istream_range<int>(stream), std::back_inserter(result));

        CHECK(result == initial);
    }

    TEST_CASE("input_from_empty_stream_is_void")
    {
        std::stringstream empty_stream;
        std::vector<double> result;
        boost::copy(burst::make_binary_istream_range<double>(empty_stream), std::back_inserter(result));

        CHECK(result.empty());
    }

    TEST_CASE("non_trivial_objects_must_provide_custom_io_functions")
    {
        std::vector<std::string> names{"Вася Пупкин", "Доздраперма Кузьминишна"};
        static_assert(not std::is_trivial<decltype(names)::value_type>::value, "");

        const auto write_string = [] (std::ostream & stream, const std::string & string) -> std::ostream &
        {
            auto string_size = string.size();
            stream.write(reinterpret_cast<const char *>(&string_size), sizeof(string_size));
            return stream.write(string.data(), static_cast<std::streamsize>(string_size));
        };

        const auto read_string = [] (std::istream & stream, std::string & string) -> std::istream &
        {
            std::size_t string_size = 0;
            if (stream.read(reinterpret_cast<char *>(&string_size), sizeof(string_size)))
            {
                string.resize(string_size);
                return stream.read(const_cast<char *>(string.data()), static_cast<std::streamsize>(string_size));
            }

            return stream;
        };

        std::stringstream stream;
        boost::copy(names, burst::make_binary_ostream_iterator(stream, write_string));

        std::vector<std::string> result;
        boost::copy(burst::make_binary_istream_range<std::string>(stream, read_string), std::back_inserter(result));

        CHECK(result == names);
    }
}
