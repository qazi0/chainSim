#ifndef CHAINSIM_PURCHASETPOP_H
#define CHAINSIM_PURCHASETPOP_H

#include "PurchasePolicy.h"
#include <QObject>
#include <cmath>

class PurchaseTPOP : public PurchasePolicy
{
    Q_OBJECT

public:
    PurchaseTPOP(quint32 leadTime, double avgDemand, quint32 reviewPeriod, QObject* parent = nullptr);

    [[nodiscard]] qint64 get_purchase(const simulation_records_t &pastRecords,
                                      quint32 current_day) const override;

    [[nodiscard]] QString name() const override;

private:
    quint32 m_lead_time;
    double m_average_daily_demand;
    quint32 m_review_period;
    double m_target_level;

    void calculate_target_level();
    void validate_parameters() const;
    [[nodiscard]] bool is_review_day(quint32 day) const;
    [[nodiscard]] QString get_calculation_details(
        const simulation_records_t &pastRecords,
        quint32 current_day) const override;
};

#endif // CHAINSIM_PURCHASETPOP_H
