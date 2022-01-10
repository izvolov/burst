// #include <utility/io/write.hpp>

#include <random/urd_order_statistic_distribution.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/math/special_functions/beta.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ostream>
#include <random>
#include <vector>

template <typename URNG, typename Integer>
auto beta (URNG && engine, Integer a, Integer b)
{
    static std::uniform_real_distribution<double> uniform(0, 1);
    double p = uniform(engine);
    return boost::math::ibeta_inv(a, b, p);
}

template <typename URNG, typename OutputIterator>
auto schema1 (URNG && engine, std::size_t range_length, std::int64_t min, std::int64_t max, OutputIterator result)
{
    std::uniform_int_distribution<std::int64_t> uniform(min, max);
        
    // std::vector<double> range(range_length);
    for (auto i = 0ul; i < range_length; ++i)
    {
        *result = uniform(engine);
        ++result;
    }
    // std::generate(range.begin(), range.end(), [&] {return uniform(engine);});

    return result;
}

template <typename URNG, typename OutputIterator>
auto schema2 (URNG && engine, std::size_t block_size, std::size_t range_length, std::int64_t min, std::int64_t max, OutputIterator result)
{
    const auto n = range_length;

    auto current_min = min;

    const auto complete_blocks = range_length / block_size;
    for (auto i = 0ul; i < complete_blocks; ++i)
    {
        const auto k = (i + 1) * block_size;

        auto b = beta(engine, k, n - k + 1);
        auto kth_stat = std::llround(b * (max - min) + min);
        // std::cout << k << " th: " << kth_stat << std::endl;

        result = schema1(engine, block_size - 1, current_min, kth_stat, result);
        *result = kth_stat;
        ++result;

        current_min = kth_stat;
    }

    const auto remainder = range_length % block_size;
    return schema1(engine, remainder, current_min, max, result);

    /////////////////

    // if (range_length <= block_size)
    // {
    //     return schema1(engine, range_length, min, max, result);
    // }

    // const auto n = range_length;
    // const auto k = n / 2;

    // auto middle_beta = beta(engine, k, n - k + 1);
    // auto middle = middle_beta * (max - min) + min;

    // // std::uniform_real_distribution<double> uniform(min, max);
    // // auto middle = uniform(engine);
    // std::cout << "m " << middle << std::endl;

    // result = schema1(engine, k - 1, min, middle, result);

    // *result = middle;
    // ++result;

    // return schema1(engine, n - k, middle, max, result);
}

template <typename URNG, typename OutputIterator>
auto schema3 (URNG && engine, std::size_t block_size, std::size_t range_length, std::int64_t min, std::int64_t max, OutputIterator result)
{
    const auto n = range_length;

    auto bounds = std::vector<std::int64_t>{min};

    auto current_min = min;

    const auto remainder = range_length % block_size;
    const auto block_count = range_length / block_size + static_cast<std::size_t>(remainder != 0);
    for (auto i = 0ul; i < block_count - 1; ++i)
    {
        const auto k = (i + 1) * block_size;

        auto osd = utility::urd_order_statistic_distribution<double>(n, k, min, max);
        const auto kth_stat = std::llround(osd(engine));

        std::cout << k << " th: " << kth_stat << std::endl;

        bounds.push_back(kth_stat);

        current_min = kth_stat;
    }
    bounds.push_back(max);

    std::sort(bounds.begin(), bounds.end());

    for (auto i = 1ul; i < bounds.size() - 1; ++i)
    {
        result = schema1(engine, block_size - 1, bounds[i - 1], bounds[i], result);
        *result = bounds[i];
        ++result;
    }
    return schema1(engine, remainder != 0 ? remainder : block_size, bounds[bounds.size() - 2], bounds[bounds.size() - 1], result);
}

template <typename Range>
void stat (std::string_view name, const Range & r)
{
    namespace ba = boost::accumulators;
    auto acc = ba::accumulator_set<double, ba::stats<ba::tag::mean, ba::tag::moment<2>>>{};
    for (auto x: r)
    {
        acc(x);
    }

    std::cout
        << name << ' '
        << ba::mean(acc) << ' '
        << ba::moment<2>(acc) << std::endl;
}

template <typename Range>
std::ostream & print (std::ostream & s, const Range & r)
{
    for (const auto & x: r)
    {
        s << x << ' ';
    }
    return s << '\n';
}

int main (int /*argc*/, const char * argv[])
{
    const auto range_length = std::stoul(argv[1]);
    const auto min = std::stol(argv[2]);
    const auto max = std::stol(argv[3]);
    const auto block_size = std::stoul(argv[4]);

    auto seed =
        static_cast<std::default_random_engine::result_type>
        (
            std::chrono::system_clock::now().time_since_epoch().count()
        );
    std::default_random_engine engine(seed);
    
    std::vector<std::int64_t> range1(range_length);
    schema1(engine, range_length, min, max, range1.begin());
    std::sort(range1.begin(), range1.end());
    for (auto i = block_size; i < range_length; i += block_size)
    {
        std::cout << i << "th: " << range1[i] << std::endl;
    }
    stat("simple", range1);

    engine.seed(seed);
    std::vector<std::int64_t> range2(range_length, 0);
    schema2(engine, block_size, range_length, min, max, range2.begin());
    // std::cout << range2[range_length / 2 - 1] << std::endl;
    // print(std::cout, range2);
    stat("n / 2", range2);

    engine.seed(seed);
    std::vector<std::int64_t> range3;//(range_length, 0);
    schema3(engine, block_size, range_length, min, max, std::back_inserter(range3));
    // print(std::cout, range3);
    stat("bounded", range3);
}
