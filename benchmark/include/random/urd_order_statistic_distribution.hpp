#ifndef BURST_BENCHMARK_UTILITY_RANDOM_URD_ORDER_STATISTIC_DISTRIBUTION_HPP
#define BURST_BENCHMARK_UTILITY_RANDOM_URD_ORDER_STATISTIC_DISTRIBUTION_HPP

#include <boost/math/special_functions/beta.hpp>

#include <cstddef>
#include <random>
#include <tuple>

namespace utility
{
    template <typename RealType>
    class urd_order_statistic_distribution;

    template <typename RealType>
    class urd_order_statistic_distribution_param
    {
    public:
        using distribution_type = urd_order_statistic_distribution<RealType>;

        urd_order_statistic_distribution_param
        (
            std::size_t n,
            std::size_t k,
            RealType a,
            RealType b
        ):
            m_n(n),
            m_k(k),
            m_a(a),
            m_b(b)
        {
        }

        std::size_t n () const
        {
            return m_n;
        }

        std::size_t k () const
        {
            return m_k;
        }

        RealType a () const
        {
            return m_a;
        }

        RealType b () const
        {
            return m_b;
        }

        friend bool
            operator ==
            (
                const urd_order_statistic_distribution_param & left,
                const urd_order_statistic_distribution_param & right
            )
        {
            return
                std::tie(left.m_n, left.m_k, left.m_a, left.m_b) ==
                std::tie(right.m_n, right.m_k, right.m_a, right.m_b);
        }

        friend bool
            operator !=
            (
                const urd_order_statistic_distribution_param & left,
                const urd_order_statistic_distribution_param & right
            )
        {
            return !(left == right);
        }

    private:
        std::size_t m_n;
        std::size_t m_k;
        RealType m_a;
        RealType m_b;
    };

    /*!
        \brief
            Распределение порядковых статистик выборки из стандартного непрерывного равномерного
            распределения.

        \details
            "urd" означает "uniform random distribution".
            https://ru.wikipedia.org/wiki/Порядковая_статистика#Пример
    */
    template <typename RealType = double>
    class urd_order_statistic_distribution
    {
    public:
        using result_type = RealType;
        using param_type = urd_order_statistic_distribution_param<result_type>;

        /*!
            \brief
                Создание распределения с параметрами

            \param n
                Размер выборки.
            \param k
                Номер порядковой статистики.
            \param a
                Минимальное значение равномерного распределения.
            \param b
                Максимальное значение равномерного распределения.
         */
        urd_order_statistic_distribution
        (
            std::size_t n,
            std::size_t k,
            result_type a,
            result_type b
        ):
            m_param(n, k, a, b)
        {
        }

        explicit urd_order_statistic_distribution (const param_type & p):
            m_param(p)
        {
        }

        void reset ()
        {
        }

        template <typename URNG>
        result_type operator () (URNG & g)
        {
            return (*this)(g, m_param);
        }

        template <typename URNG>
        result_type operator () (URNG & g, const param_type & p)
        {
            auto ibeta = m_uniform(g);
            auto beta = boost::math::ibeta_inv(p.k(), p.n() - p.k() + 1, ibeta);
            return beta * (p.b() - p.a()) + p.a();
        }

        std::size_t n () const
        {
            return m_param.n();
        }

        std::size_t k () const
        {
            return m_param.k();
        }

        result_type a () const
        {
            return m_param.a();
        }

        result_type b () const
        {
            return m_param.b();
        }

        param_type param () const
        {
            return m_param;
        }

        void param (const param_type & p)
        {
            m_param = p;
        }

        result_type min () const
        {
            return a();
        }

        result_type max () const
        {
            return b();
        }

        friend bool
            operator ==
            (
                const urd_order_statistic_distribution & left,
                const urd_order_statistic_distribution& right
            )
        {
            return left.m_param == right.m_param;
        }

        friend bool
            operator !=
            (
                const urd_order_statistic_distribution & left,
                const urd_order_statistic_distribution& right
            )
        {
            return !(left == right);
        }

    private:
        using uniform_real_distribution_type = std::uniform_real_distribution<result_type>;

        param_type m_param;
        uniform_real_distribution_type m_uniform{0, 1};
    };
}

#endif // BURST_BENCHMARK_UTILITY_RANDOM_URD_ORDER_STATISTIC_DISTRIBUTION_HPP
