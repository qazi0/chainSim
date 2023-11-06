
#ifndef CHAINSIM_PURCHASEROP_H
#define CHAINSIM_PURCHASEROP_H

#include "PurchasePolicy.h"

class PurchaseROP : public PurchasePolicy {
    unsigned m_lead_time;
    double m_average_daily_demand;
    double m_safety_stock;
    long m_reorder_point;

public:
    PurchaseROP(unsigned leadTime, double avgDemand);

    [[nodiscard]] long
    get_purchase(const simulation_records_t &pastRecords,
                 unsigned current_day) const final;

    [[nodiscard]] std::string name() const override;
};


#endif //CHAINSIM_PURCHASEROP_H
