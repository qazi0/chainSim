#ifndef CHAINSIM_CHAINSIMBUILDER_H
#define CHAINSIM_CHAINSIMBUILDER_H

#include <QObject>
#include <QString>
#include <memory>
#include "ChainSim.h"

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
        ChainSimBuilder &setStartingInventory(quint64 startingInventory);
        ChainSimBuilder &setLoggingLevel(quint32 loggingLevel);

        // Create and return a new ChainSim instance
        std::unique_ptr<ChainSim> create();

    private:
        void validateConfiguration() const;

        QString m_simulation_name;
        quint64 m_simulation_length{30};
        quint64 m_lead_time{5};
        double m_current_demand{50.0};
        quint64 m_starting_inventory{0};
        quint32 m_logging_level{0};
    };
}

#endif // CHAINSIM_CHAINSIMBUILDER_H
