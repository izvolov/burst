#include <burst/range/intersect.hpp>
#include <io.hpp>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range.hpp>

#include <iostream>
#include <numeric>
#include <vector>

template <typename Container>
void test_on_the_fly_intersect (const Container & values)
{
    using nested_container_type = typename Container::value_type;

    std::vector<boost::iterator_range<typename nested_container_type::const_iterator>> ranges;
    boost::for_each(values,
        [& ranges] (const nested_container_type & values)
        {
            ranges.push_back(boost::make_iterator_range(values));
        });

    clock_t intersect_time = clock();
    auto intersected_range = burst::intersect(ranges);
    auto distance = static_cast<std::size_t>(std::distance(intersected_range.begin(), intersected_range.end()));
    intersect_time = clock() - intersect_time;

    std::cout << "Пересечение на лету: " << distance << std::endl;
    std::cout << "\t" << static_cast<double>(intersect_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << std::endl;
}

template <typename Container>
void test_std_intersect (const Container & values)
{
    typename Container::value_type intersected(values.front());

    clock_t intersect_time = clock();
    for (const auto & row: boost::make_iterator_range(std::next(values.begin()), values.end()))
    {
        typename Container::value_type result;
        result.reserve(intersected.size() + row.size());
        std::set_intersection(intersected.begin(), intersected.end(), row.begin(), row.end(), std::back_inserter(result));
        std::swap(result, intersected);
    }
    intersect_time = clock() - intersect_time;

    std::cout << "std::set_intersection: " << intersected.size() << std::endl;
    std::cout << "\t" << static_cast<double>(intersect_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << std::endl;
}

int main ()
{
    std::vector<std::vector<std::int64_t>> values;
    read_many(std::cin, values);

    test_std_intersect(values);
    test_on_the_fly_intersect(values);
}
