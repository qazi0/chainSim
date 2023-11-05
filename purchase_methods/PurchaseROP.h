//
// Created by blakberrisigma on 03/11/2023.
//

#ifndef CHAINSIM_PURCHASEROP_H
#define CHAINSIM_PURCHASEROP_H

#include "PurchaseMethod.h"

class PurchaseROP : public PurchaseMethod {
    unsigned m_lead_time;
    double m_average_daily_demand;
    double m_safety_stock;
    long m_reorder_point;

public:
    PurchaseROP(unsigned leadTime, double avgDemand);

    [[nodiscard]] long
    get_purchase(const std::unordered_map<std::string, std::vector<long>> &pastRecords,
                 unsigned current_day) const final;

    std::string getName() const override;
};


#endif //CHAINSIM_PURCHASEROP_H
