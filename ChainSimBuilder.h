#ifndef CHAINSIM_CHAINSIMBUILDER_H
#define CHAINSIM_CHAINSIMBUILDER_H

#include <QObject>
#include <QString>
#include <memory>
#include "ChainSim.h"
#include "utils/DemandSampler.hpp"

namespace qz
{
    class ChainSimBuilder : public QObject
    {
        Q_OBJECT

    public:
        explicit ChainSimBuilder(QObject *parent = nullptr);

        // Configuration methods
        ChainSimBuilder &setSimulationName(const QString &simulationName);
        ChainSimBuilder &setSimulationLength(quint64 simulationLength);
        ChainSimBuilder &setLeadTime(quint64 leadTime);
        ChainSimBuilder &setAverageDemand(double averageDemand);
        ChainSimBuilder &setDemandStdDev(double stdDev);
        ChainSimBuilder &setDeterministic(bool deterministic);
        ChainSimBuilder &setSeed(unsigned seed);
        ChainSimBuilder &setStartingInventory(quint64 startingInventory);
        ChainSimBuilder &setLoggingLevel(quint32 loggingLevel);
        ChainSimBuilder &setDemandDistribution(const QString &distribution);
        ChainSimBuilder &setGammaParameters(double shape, double scale);
        ChainSimBuilder &setUniformParameters(double min, double max);

        // Create and return a new ChainSim instance
        std::unique_ptr<ChainSim> create();

    private:
        void validateConfiguration() const;

        QString m_simulation_name;
        quint64 m_simulation_length{30};
        quint64 m_lead_time{5};
        double m_average_demand{50.0};
        double m_demand_stddev{10.0};
        bool m_deterministic{false};
        unsigned m_seed{7};
        quint64 m_starting_inventory{0};
        quint32 m_logging_level{0};
        QString m_demand_distribution{"normal"};
        double m_gamma_shape{1.0};
        double m_gamma_scale{1.0};
        double m_uniform_min{0.0};
        double m_uniform_max{100.0};
    };
}

#endif // CHAINSIM_CHAINSIMBUILDER_H
