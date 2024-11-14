#ifndef CHAINSIM_CHAINSIM_H
#define CHAINSIM_CHAINSIM_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QDebug>
#include <memory>

#include "purchase_policies/PurchasePolicy.h"
#include "utils/ChainLogger.hpp"
#include "utils/DemandSampler.hpp"

namespace qz
{
    class ChainSim : public QObject
    {
        Q_OBJECT

    public:
        using simulation_records_t = QMap<QString, QVector<qint64>>;

        void initialize_simulation();

        // Simulate entire duration
        void simulate(const PurchasePolicy &purchasePolicy);

        // Simulate specific number of days
        void simulate_days(const PurchasePolicy &purchasePolicy, quint64 days);

        // Simulate single day
        void simulate_day(const PurchasePolicy &purchasePolicy, quint64 day);

        [[nodiscard]] simulation_records_t get_simulation_records() const;
        [[nodiscard]] quint64 get_current_day() const { return m_current_day; }

    Q_SIGNALS:
        void simulationStarted();
        void simulationFinished();
        void daySimulated(quint64 day);
        void errorOccurred(const QString &error);

    private:
        ChainSim();
        friend class ChainSimBuilder;

        quint64 m_simulation_length{};
        quint64 m_starting_inventory{};
        quint64 m_lead_time{};
        std::unique_ptr<DemandSampler> m_demandSampler;
        quint64 m_current_day{1}; // Start from day 1

        QString m_simulation_name;
        QVector<QString> m_records_columns;
        simulation_records_t m_records;

        quint32 m_logging_level{0};
        ChainLogger m_logger{};
    };
}

#endif // CHAINSIM_CHAINSIM_H
