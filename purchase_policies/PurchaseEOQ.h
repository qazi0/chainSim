#ifndef CHAINSIM_PURCHASEEOQ_H
#define CHAINSIM_PURCHASEEOQ_H

#include "PurchasePolicy.h"
#include <cmath>
#include <stdexcept>

class PurchaseEOQ : public PurchasePolicy
{
public:
    PurchaseEOQ(unsigned leadTime, double avgDemand, double orderingCost, double holdingCostRate);

    [[nodiscard]] long get_purchase(const simulation_records_t &pastRecords,
                                    unsigned current_day) const override;

    [[nodiscard]] std::string name() const override;

private:
    unsigned m_lead_time;
    double m_average_daily_demand;
    double m_ordering_cost;
    double m_holding_cost_rate;
    double m_eoq;
    double m_reorder_point;

    void calculate_eoq();
    void validate_parameters() const;
    [[nodiscard]] std::string get_calculation_details(
        const simulation_records_t &pastRecords,
        unsigned current_day) const override;
};

#endif // CHAINSIM_PURCHASEEOQ_H
