#ifndef CHAINSIM_DEMANDSAMPLER_HPP
#define CHAINSIM_DEMANDSAMPLER_HPP

#include <random>
#include <memory>

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
            // Keep sampling until we get a non-negative value
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

} // namespace qz

#endif // CHAINSIM_DEMANDSAMPLER_HPP