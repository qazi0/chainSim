#ifndef CHAINSIM_PURCHASEEOQ_H
#define CHAINSIM_PURCHASEEOQ_H

#include "PurchasePolicy.h"
#include <QObject>
#include <cmath>

class PurchaseEOQ : public PurchasePolicy
{
    Q_OBJECT

public:
    PurchaseEOQ(quint32 leadTime,
                double avgDemand,
                double orderingCost,
                double holdingCostRate,
                QObject *parent = nullptr);

    [[nodiscard]] qint64 get_purchase(const simulation_records_t &pastRecords,
                                      quint32 current_day) const override;

    [[nodiscard]] QString name() const override;

private:
    quint32 m_lead_time;
    double m_average_daily_demand;
    double m_ordering_cost;
    double m_holding_cost_rate;
    double m_eoq;
    qint64 m_reorder_point;

    void calculate_eoq();
    void validate_parameters() const;
    [[nodiscard]] QString get_calculation_details(
        const simulation_records_t &pastRecords,
        quint32 current_day) const override;
};

#endif // CHAINSIM_PURCHASEEOQ_H
