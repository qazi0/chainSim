#ifndef CHAINSIM_PURCHASEROP_H
#define CHAINSIM_PURCHASEROP_H

#include "PurchasePolicy.h"
#include <QObject>

/* Re-Order Point (ROP)
 * Ref: https://manufacturing-software-blog.mrpeasy.com/what-is-reorder-point-and-reorder-point-formula/
 */
class PurchaseROP : public PurchasePolicy
{
    Q_OBJECT

private:
    quint32 m_lead_time;
    double m_average_daily_demand;
    double m_safety_stock;
    qint64 m_reorder_point;

public:
    PurchaseROP(quint32 leadTime, double avgDemand, QObject *parent = nullptr);

    [[nodiscard]] qint64
    get_purchase(const simulation_records_t &pastRecords,
                 quint32 current_day) const final;

    [[nodiscard]] QString name() const final;

    [[nodiscard]] QString get_calculation_details(
        const simulation_records_t &pastRecords,
        quint32 current_day) const override;
};

#endif // CHAINSIM_PURCHASEROP_H
