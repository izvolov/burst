#include <iostream>
#include <numeric>
#include <vector>

#include <boost/assert.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <burst/range/merge.hpp>
#include <burst/iterator/merge_iterator.hpp>
#include <io.hpp>

template <typename Container>
void test_on_the_fly_merge (const Container & values)
{
    using nested_container_type = typename Container::value_type;

    std::vector<boost::iterator_range<typename nested_container_type::const_iterator>> ranges;
    boost::for_each(values,
        [& ranges] (const nested_container_type & values)
        {
            ranges.push_back(boost::make_iterator_range(values));
        });

    clock_t merge_time = clock();
    auto merged_range = burst::merge(ranges);
    auto distance = static_cast<std::size_t>(std::distance(merged_range.begin(), merged_range.end()));
    merge_time = clock() - merge_time;

    BOOST_VERIFY
    (
        distance ==
            std::accumulate(values.begin(), values.end(), 0ul,
                [] (std::size_t current_value, const nested_container_type & vector)
                {
                    return current_value + vector.size();
                })
    );

    std::cout << "Слияние на лету:" << std::endl;
    std::cout << "\t" << static_cast<double>(merge_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << std::endl;
}

template <typename Container>
void test_merge_by_sorting (const Container & values)
{
    clock_t merge_time = clock();

    typename Container::value_type flat_values;
    for (const auto & row: values)
    {
        flat_values.insert(flat_values.end(), row.begin(), row.end());
    }

    std::sort(flat_values.begin(), flat_values.end());

    merge_time = clock() - merge_time;

    std::cout << "Слияние сортировкой:" << std::endl;
    std::cout << "\t" << static_cast<double>(merge_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << std::endl;
}

template <typename Container>
void test_std_merge (const Container & values)
{
    typename Container::value_type merged;

    clock_t merge_time = clock();
    for (const auto & row: values)
    {
        typename Container::value_type result;
        result.reserve(merged.size() + row.size());
        std::merge(merged.begin(), merged.end(), row.begin(), row.end(), std::back_inserter(result));
        std::swap(result, merged);
    }
    merge_time = clock() - merge_time;

    std::cout << "std::merge:" << std::endl;
    std::cout << "\t" << static_cast<double>(merge_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << std::endl;
}

int main ()
{
    std::vector<std::vector<std::int64_t>> values;
    read_many(std::cin, values);

    test_merge_by_sorting(values);
    test_std_merge(values);
    test_on_the_fly_merge(values);
}
