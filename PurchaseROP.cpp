//
// Created by blakberrisigma on 03/11/2023.
//

#include "PurchaseROP.h"

PurchaseROP::PurchaseROP(unsigned leadTime, double avgDemand) :
        m_lead_time{leadTime}, m_average_daily_demand{avgDemand} {

    m_safety_stock = std::ceil(m_average_daily_demand) * leadTime;
    m_reorder_point = static_cast<long long>(m_average_daily_demand * leadTime + m_safety_stock);
}

long PurchaseROP::get_purchase(const std::unordered_map<std::string, std::vector<long>> &pastRecords,
                               unsigned current_day) const {
    auto current_inventory = pastRecords.at("inventory_quantity")[current_day];
    long reorder_quantity{0};
    if (current_inventory <= m_reorder_point)
        reorder_quantity = std::ceil(m_average_daily_demand * m_lead_time);

    return reorder_quantity;
}

std::string PurchaseROP::getName() const {
    return "ROP/CR";
}
