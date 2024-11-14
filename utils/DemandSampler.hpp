#ifndef CHAINSIM_DEMANDSAMPLER_HPP
#define CHAINSIM_DEMANDSAMPLER_HPP

#include <random>
#include <memory>
#include <cmath>

namespace qz
{

    class DemandSampler
    {
    public:
        virtual ~DemandSampler() = default;
        virtual double sample() = 0;
        [[nodiscard]] virtual double getMean() const = 0;
    };

    class FixedDemandSampler : public DemandSampler
    {
    public:
        explicit FixedDemandSampler(double fixedDemand)
            : m_fixedDemand(fixedDemand) {}

        double sample() override { return m_fixedDemand; }
        [[nodiscard]] double getMean() const override { return m_fixedDemand; }

    private:
        double m_fixedDemand;
    };

    class NormalDemandSampler : public DemandSampler
    {
    public:
        NormalDemandSampler(double mean, double stddev, unsigned seed)
            : m_mean(mean), m_stddev(stddev), m_generator(seed),
              m_distribution(mean, stddev) {}

        double sample() override
        {
            double value;
            do
            {
                value = m_distribution(m_generator);
            } while (value < 0);
            return value;
        }

        [[nodiscard]] double getMean() const override { return m_mean; }

    private:
        double m_mean;
        double m_stddev;
        std::mt19937 m_generator;
        std::normal_distribution<double> m_distribution;
    };

    class GammaDemandSampler : public DemandSampler
    {
    public:
        GammaDemandSampler(double shape, double scale, unsigned seed)
            : m_shape(shape), m_scale(scale), m_generator(seed),
              m_distribution(shape, scale) {}

        double sample() override
        {
            return m_distribution(m_generator);
        }

        [[nodiscard]] double getMean() const override { return m_shape * m_scale; }

    private:
        double m_shape;
        double m_scale;
        std::mt19937 m_generator;
        std::gamma_distribution<double> m_distribution;
    };

    class PoissonDemandSampler : public DemandSampler
    {
    public:
        explicit PoissonDemandSampler(double mean, unsigned seed)
            : m_mean(mean), m_generator(seed),
              m_distribution(mean) {}

        double sample() override
        {
            return static_cast<double>(m_distribution(m_generator));
        }

        [[nodiscard]] double getMean() const override { return m_mean; }

    private:
        double m_mean;
        std::mt19937 m_generator;
        std::poisson_distribution<int> m_distribution;
    };

    class UniformDemandSampler : public DemandSampler
    {
    public:
        UniformDemandSampler(double min, double max, unsigned seed)
            : m_min(min), m_max(max), m_generator(seed),
              m_distribution(min, max) {}

        double sample() override
        {
            return m_distribution(m_generator);
        }

        [[nodiscard]] double getMean() const override { return (m_max + m_min) / 2.0; }

    private:
        double m_min;
        double m_max;
        std::mt19937 m_generator;
        std::uniform_real_distribution<double> m_distribution;
    };

} // namespace qz

#endif // CHAINSIM_DEMANDSAMPLER_HPP