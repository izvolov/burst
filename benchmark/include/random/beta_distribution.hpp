#ifndef BURST_BENCHMARK_UTILITY_RANDOM_BETA_DISTRIBUTION_HPP
#define BURST_BENCHMARK_UTILITY_RANDOM_BETA_DISTRIBUTION_HPP

namespace utility
{
    template <typename RealType>
    class beta_distribution_param
    {
    public:
        using distribution_type = beta_distribution<RealType>;

        param_type (std::size_t n, std::size_t k, RealType a, RealType b):
            n(n),
            k(k),
            a(a),
            b(b)
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

        RealType a () const
        {
            return m_b;
        }

        friend bool operator == (const param_type & left, const param_type & right)
        {
            return
                std::tie(left.m_n, left.m_k, left.m_a, left.m_b) ==
                std::tie(right.m_n, right.m_k, right.m_a, right.m_b);
        }

        friend bool operator != (const param_type & left, const param_type & right)
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
    */
    template <typename RealType = double>
    class beta_distribution
    {
    public:
        using result_type = RealType;
        using param_type = beta_distribution_param<result_type>;

        beta_distribution (std::size_t n, std::size_t k, RealType a, RealType b):
            m_param(n, k, a, b)
        {
        }

        beta_distribution (const param_type & p):
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
        result_type operator () (URNG & g, const param_type& p);

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
                const beta_distribution & left,
                const beta_distribution& right
            )
        {
            return left.m_param == right.m_param;
        }

        friend bool
            operator !=
            (
                const beta_distribution & left,
                const beta_distribution& right
            )
        {
            return !(left == right);
        }

    private:
        param_type m_param;
    }

    template <typename URNG, typename Integer>
    auto beta (URNG && engine, Integer a, Integer b)
    {
        static std::uniform_real_distribution<double> uniform(0, 1);
        double p = uniform(engine);
        return boost::math::ibeta_inv(a, b, p);
    }
}

#endif // BURST_BENCHMARK_UTILITY_RANDOM_BETA_DISTRIBUTION_HPP
