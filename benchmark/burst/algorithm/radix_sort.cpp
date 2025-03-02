#include <utility/io/read.hpp>

#include <burst/algorithm/radix_sort.hpp>
#include <burst/string/u8s.hpp>
#include <burst/integer/to_ordered_integral.hpp>

#include <boost/program_options.hpp>
#include <boost/sort/spreadsort/spreadsort.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using burst::literals::operator ""_u8s;

const auto burst_radix_sort_call_name = std::string("burst::radix_sort");
const auto burst_radix_sort_par_max_call_name = std::string("burst::radix_sort(par(max))");
const auto burst_radix_sort_par_2_call_name = std::string("burst::radix_sort(par(2))");
const auto burst_radix_sort_par_4_call_name = std::string("burst::radix_sort(par(4))");
const auto burst_radix_sort_par_8_call_name = std::string("burst::radix_sort(par(8))");
const auto burst_radix_sort_par_16_call_name = std::string("burst::radix_sort(par(16))");
const auto std_sort_call_name = std::string("std::sort");
const auto std_stable_sort_call_name = std::string("std::stable_sort");
const auto boost_spreadsort_call_name = std::string("boost::spreadsort");

const auto burst_radix_sort_title = std::string("radix");
const auto burst_radix_sort_par_max_title = std::string("par");
const auto burst_radix_sort_par_2_title = std::string("par2");
const auto burst_radix_sort_par_4_title = std::string("par4");
const auto burst_radix_sort_par_8_title = std::string("par8");
const auto burst_radix_sort_par_16_title = std::string("par16");
const auto std_sort_title = std::string("std");
const auto std_stable_sort_title = std::string("stable");
const auto boost_spreadsort_title = std::string("boost");

const auto default_algorithms_set =
    std::vector<std::string>
    {
        burst_radix_sort_title,
        burst_radix_sort_par_max_title,
        std_sort_title,
        std_stable_sort_title,
        boost_spreadsort_title
    };

using clock_type = std::chrono::steady_clock;

template <typename Sort, typename Container, typename UnaryFunction1, typename UnaryFunction2>
void
    test_sort
    (
        const std::string & name,
        Sort sort,
        const Container & numbers,
        std::size_t attempts,
        UnaryFunction1 statistic,
        UnaryFunction2 prepare
    )
{
    using namespace std::chrono;
    std::vector<clock_type::duration> times;
    times.reserve(attempts);

    for (std::size_t attempt = 0; attempt < attempts; ++attempt)
    {
        auto unsorted = prepare(numbers);

        auto attempt_start_time = clock_type::now();
        sort(unsorted.begin(), unsorted.end());
        auto attempt_time = clock_type::now() - attempt_start_time;
        times.push_back(attempt_time);
    }

    const auto stat = statistic(times);
    std::cout << name << ' ' << duration_cast<duration<double>>(stat).count() << std::endl;
}

template <typename Value, typename UnaryFunction1, typename UnaryFunction2>
void
    test_all
    (
        const std::vector<std::string> & to_bench,
        std::size_t attempts,
        UnaryFunction1 statistic,
        UnaryFunction2 prepare
    )
{
    std::vector<Value> numbers;
    utility::read(std::cin, numbers);

    std::vector<Value> buffer(numbers.size());

    auto radix_sort =
        [& buffer] (auto && ... args)
        {
            return burst::radix_sort(std::forward<decltype(args)>(args)..., buffer.begin(), burst::to_ordered_integral);
        };
    auto radix_sort_par_max =
        [& buffer] (auto && ... args)
        {
            const auto par = burst::par(std::max(std::thread::hardware_concurrency(), 2u));
            return burst::radix_sort(par, std::forward<decltype(args)>(args)..., buffer.begin(), burst::to_ordered_integral);
        };
    auto radix_sort_par_2 =
        [& buffer] (auto && ... args)
        {
            return burst::radix_sort(burst::par(2), std::forward<decltype(args)>(args)..., buffer.begin(), burst::to_ordered_integral);
        };
    auto radix_sort_par_4 =
        [& buffer] (auto && ... args)
        {
            return burst::radix_sort(burst::par(4), std::forward<decltype(args)>(args)..., buffer.begin(), burst::to_ordered_integral);
        };
    auto radix_sort_par_8 =
        [& buffer] (auto && ... args)
        {
            return burst::radix_sort(burst::par(8), std::forward<decltype(args)>(args)..., buffer.begin(), burst::to_ordered_integral);
        };
    auto radix_sort_par_16 =
        [& buffer] (auto && ... args)
        {
            return burst::radix_sort(burst::par(16), std::forward<decltype(args)>(args)..., buffer.begin(), burst::to_ordered_integral);
        };
    auto std_sort =
        [] (auto && ... args)
        {
            return std::sort(std::forward<decltype(args)>(args)...);
        };
    auto std_stable_sort =
        [] (auto && ... args)
        {
            return std::stable_sort(std::forward<decltype(args)>(args)...);
        };
    auto boost_spreadsort =
        [] (auto && ... args)
        {
            return boost::sort::spreadsort::spreadsort(std::forward<decltype(args)>(args)...);
        };

    using iterator_type = typename std::vector<Value>::iterator;
    using sort_call_type = std::function<void(iterator_type, iterator_type)>;
    auto sort_calls =
        std::unordered_map<std::string, std::pair<std::string, sort_call_type>>
        {
            {burst_radix_sort_title, {burst_radix_sort_call_name, radix_sort}},
            {burst_radix_sort_par_max_title, {burst_radix_sort_par_max_call_name, radix_sort_par_max}},
            {burst_radix_sort_par_2_title, {burst_radix_sort_par_2_call_name, radix_sort_par_2}},
            {burst_radix_sort_par_4_title, {burst_radix_sort_par_4_call_name, radix_sort_par_4}},
            {burst_radix_sort_par_8_title, {burst_radix_sort_par_8_call_name, radix_sort_par_8}},
            {burst_radix_sort_par_16_title, {burst_radix_sort_par_16_call_name, radix_sort_par_16}},
            {std_sort_title, {std_sort_call_name, std_sort}},
            {std_stable_sort_title, {std_stable_sort_call_name, std_stable_sort}},
            {boost_spreadsort_title, {boost_spreadsort_call_name, boost_spreadsort}}
        };

    for (const auto & sort_call_title: to_bench)
    {
        auto call_params = sort_calls.find(sort_call_title);
        if (call_params != sort_calls.end())
        {
            const auto & name = call_params->second.first;
            const auto & call = call_params->second.second;
            test_sort(name, call, numbers, attempts, statistic, prepare);
        }
        else
        {
            const auto error_message =
                u8"Неверный тип тестируемого алгоритма: "_u8s + sort_call_title;
            throw boost::program_options::error(error_message);
        }
    }
}

using test_call_type = std::function<void (const std::vector<std::string> &, std::size_t)>;

struct shuffle_fn
{
    template <typename Container>
    auto operator () (Container c)
    {
        std::shuffle(c.begin(), c.end(), std::mt19937((*rd)()));
        return c;
    }

    std::shared_ptr<std::random_device> rd = std::make_shared<std::random_device>();
};

template <typename BinaryPredicate>
struct sort_fn
{
    template <typename Container>
    auto operator () (Container c) const
    {
        std::sort(c.begin(), c.end(), compare);
        return c;
    }

    BinaryPredicate compare = BinaryPredicate{};
};

// Сортирует диапазон и ставит его максимальное значение на случайную позицию.
struct outlier_fn
{
    template <typename Container>
    auto operator () (Container c)
    {
        if (c.size() > 1)
        {
            std::sort(c.begin(), c.end());

            std::default_random_engine engine((*rd)());
            const auto max_outlier_index = std::distance(c.begin(), c.end()) - 2;
            using difference_type = typename Container::difference_type;
            auto uniform = std::uniform_int_distribution<difference_type>(0, max_outlier_index);
            auto outlier_index = uniform(engine);
            std::rotate(c.begin() + outlier_index, c.end() - 1, c.end());
        }
        return c;
    }

    std::shared_ptr<std::random_device> rd = std::make_shared<std::random_device>();
};

// Сортирует первую половину по возрастанию, а вторую — по убыванию.
struct pipe_organ_fn
{
    template <typename Container>
    auto operator () (Container c) const
    {
        std::sort(c.begin(), c.end());
        std::reverse(c.begin() + std::distance(c.begin(), c.end()) / 2, c.end());
        return c;
    }
};

// Делает все элементы диапазона равными одному (случайному) из тех, что были в этом диапазоне.
struct single_fn
{
    template <typename Container>
    auto operator () (Container c)
    {
        if (c.size() > 1)
        {
            std::default_random_engine engine((*rd)());
            using size_type = typename Container::size_type;
            auto uniform = std::uniform_int_distribution<size_type>(0, c.size() - 1);
            auto item = c[uniform(engine)];
            std::fill(c.begin(), c.end(), item);
        }
        return c;
    }

    std::shared_ptr<std::random_device> rd = std::make_shared<std::random_device>();
};

struct min_fn
{
    template <typename Container>
    auto operator () (const Container & c) const
    {
        return *std::min_element(c.begin(), c.end());
    }
};

struct max_fn
{
    template <typename Container>
    auto operator () (const Container & c) const
    {
        return *std::max_element(c.begin(), c.end());
    }
};

struct sum_fn
{
    template <typename Container>
    auto operator () (Container c) const
    {
        using value_type = typename Container::value_type;
        std::sort(c.begin(), c.end());
        return std::accumulate(c.begin(), c.end(), value_type{});
    }
};

struct median_fn
{
    template <typename Container>
    auto operator () (Container c) const
    {
        using difference_type = typename Container::difference_type;
        auto n = static_cast<difference_type>(c.size() / 2);
        auto nth = c.begin() + n;
        std::nth_element(c.begin(), nth, c.end());
        return *nth;
    }
};

struct mean_fn
{
    template <typename Container>
    auto operator () (const Container & c) const
    {
        using rep_type = clock_type::rep;
        return sum_fn{}(c) / static_cast<rep_type>(c.size());
    }
};

template <typename Value, typename UnaryFunction1, typename UnaryFunction2>
test_call_type make_test_all (UnaryFunction1 statistic, UnaryFunction2 prepare)
{
    return
        [statistic, prepare]
            (const std::vector<std::string> & algorithm_set, std::size_t attempts)
            {
                return test_all<Value>(algorithm_set, attempts, statistic, prepare);
            };
}

template <typename T>
using containter_for_t = std::vector<T>;

template <typename Value>
using prepare_function_for_t = std::function<containter_for_t<Value> (containter_for_t<Value>)>;

template <typename Value>
prepare_function_for_t<Value> prepare_for (const std::string & prepare_type)
{
    static const auto prepare_functions =
        std::unordered_map<std::string, prepare_function_for_t<Value>>
        {
            {"shuffle", shuffle_fn{}},
            {"ascending", sort_fn<std::less<>>{}},
            {"descending", sort_fn<std::greater<>>{}},
            {"outlier", outlier_fn{}},
            {"pipe-organ", pipe_organ_fn{}},
            {"single", single_fn{}}
        };

    auto prepare_function = prepare_functions.find(prepare_type);
    if (prepare_function != prepare_functions.end())
    {
        return prepare_function->second;
    }
    else
    {
        const auto error_message = u8"Неверный тип подготовки массива: "_u8s + prepare_type;
        throw boost::program_options::error(error_message);
    }
}

template <typename Duration>
using statistic_function_for_t = std::function<Duration (containter_for_t<Duration>)>;

template <typename Duration>
statistic_function_for_t<Duration> statistic_for (const std::string & statistic)
{
    static const auto statistic_functions =
        std::unordered_map<std::string, statistic_function_for_t<Duration>>
        {
            {"min", min_fn{}},
            {"max", max_fn{}},
            {"mean", mean_fn{}},
            {"median", median_fn{}},
            {"sum", sum_fn{}}
        };

    auto stat_function = statistic_functions.find(statistic);
    if (stat_function != statistic_functions.end())
    {
        return stat_function->second;
    }
    else
    {
        auto error_message = u8"Неверная статистика: "_u8s + statistic;
        throw boost::program_options::error(error_message);
    }
}

using dispatch_parameters_call_type =
    std::function<test_call_type (const std::string &, const std::string &)>;

template <typename Value>
test_call_type dispatch_parameters (const std::string & statistic, const std::string & prepare_type)
{
    auto statistic_function = statistic_for<clock_type::duration>(statistic);
    auto prepare_funcion = prepare_for<Value>(prepare_type);

    return make_test_all<Value>(statistic_function, prepare_funcion);
}

dispatch_parameters_call_type dispatch_type (const std::string & value_type)
{
    static const auto dispatch_statistic_calls =
        std::unordered_map<std::string, dispatch_parameters_call_type>
        {
            {"uint8", &dispatch_parameters<std::uint8_t>},
            {"uint16", &dispatch_parameters<std::uint16_t>},
            {"uint32", &dispatch_parameters<std::uint32_t>},
            {"uint64", &dispatch_parameters<std::uint64_t>},
            {"int8", &dispatch_parameters<std::int8_t>},
            {"int16", &dispatch_parameters<std::int16_t>},
            {"int32", &dispatch_parameters<std::int32_t>},
            {"int64", &dispatch_parameters<std::int64_t>},
            {"float", &dispatch_parameters<float>},
            {"double", &dispatch_parameters<double>}
        };

    auto call = dispatch_statistic_calls.find(value_type);
    if (call != dispatch_statistic_calls.end())
    {
        return call->second;
    }
    else
    {
        auto error_message = u8"Неверная разрядность сортируемых чисел: "_u8s + value_type;
        throw boost::program_options::error(error_message);
    }
}

test_call_type
    dispatch_call
    (
        const std::string & value_type,
        const std::string & statistic,
        const std::string & prepare_type
    )
{
    return dispatch_type(value_type)(statistic, prepare_type);
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("attempts", bpo::value<std::size_t>()->default_value(1000),
            "Количество испытаний")
        ("type", bpo::value<std::string>()->default_value("uint32"),
            "Тип сортируемых чисел.\n"
            "Допустимые значения: uint8, uint16, uint32, uint64, int8, int16, int32, int64, "
            "float, double")
        ("prepare", bpo::value<std::string>()->required(),
            "Тип подготовки массива перед каждым испытанием.\n"
            "Допустимые значения: shuffle, ascending, descending, outlier, pipe-organ")
        ("stat", bpo::value<std::string>()->default_value("mean"),
            "Статистика, которая будет рассчитана.\n"
            "Допустимые значения: min, max, mean, median, sum")
        ("algo", bpo::value<std::vector<std::string>>()->multitoken()
            ->default_value(default_algorithms_set, "radix par std stable boost"),
            "Набор тестируемых алгоритмов.\n"
            "Допустимые значения: radix, std, stable, boost, par, par2, par4, par8, par16");

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
            std::size_t attempts = vm["attempts"].as<std::size_t>();
            auto value_type = vm["type"].as<std::string>();
            auto statistic = vm["stat"].as<std::string>();
            auto prepare_type = vm["prepare"].as<std::string>();
            auto algorithm_set = vm["algo"].as<std::vector<std::string>>();

            auto test = dispatch_call(value_type, statistic, prepare_type);
            test(algorithm_set, attempts);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
