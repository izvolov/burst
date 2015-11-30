#include <iostream>
#include <vector>

#include <burst/iterator/subset_iterator.hpp>
#include <input.hpp>

template <typename Container>
void test (const Container & numbers)
{
    auto begin = burst::make_subset_iterator(numbers);
    auto end = burst::make_subset_iterator(numbers, burst::iterator::end_tag);

    clock_t total_time = 0;
    std::size_t iteration_count = 0;
    while (begin != end)
    {
        clock_t increment_time = clock();
        ++begin;
        increment_time = clock() - increment_time;
        total_time += increment_time;
        ++iteration_count;
    }

    std::cout << "Общее время: " << static_cast<double>(total_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << "Среднее время: " << static_cast<double>(total_time) / static_cast<double>(iteration_count) / CLOCKS_PER_SEC << std::endl;
}

int main ()
{
    std::vector<std::int64_t> numbers;
    read(std::cin, numbers);

    test(numbers);
}
