//
// Created by blakberrisigma on 03/11/2023.
//

#include "PurchaseROP.h"

PurchaseROP::PurchaseROP(unsigned leadTime, double avgDemand) : m_lead_time{leadTime}, m_average_daily_demand{avgDemand}
{
    if (leadTime == 0)
    {
        throw std::invalid_argument("Lead time must be greater than zero");
    }
    if (avgDemand <= 0)
    {
        throw std::invalid_argument("Average demand must be positive");
    }
    m_safety_stock = std::ceil(m_average_daily_demand) * leadTime;
    m_reorder_point = static_cast<long>(m_average_daily_demand * leadTime + m_safety_stock);
}

long PurchaseROP::get_purchase(const std::unordered_map<std::string, std::vector<long>> &pastRecords,
                               unsigned current_day) const
{
    auto current_inventory = pastRecords.at("inventory_quantity")[current_day];
    long reorder_quantity{0};
    if (current_inventory <= m_reorder_point)
        reorder_quantity = std::ceil(m_average_daily_demand * m_lead_time);

    return reorder_quantity;
}

std::string PurchaseROP::name() const
{
    return "ROP/CR";
}

std::string PurchaseROP::get_calculation_details(
    const simulation_records_t &pastRecords,
    unsigned current_day) const
{

    auto current_inventory = pastRecords.at("inventory_quantity")[current_day];
    std::stringstream ss;

    auto order_qty = (current_inventory <= m_reorder_point) ? std::ceil(m_average_daily_demand * m_lead_time) : 0;

    ss << "ROP = LT×D + SS = " << m_lead_time << "×" << m_average_daily_demand
       << " + " << m_safety_stock << " = " << m_reorder_point << "\n\t  "
       << "INV = " << current_inventory << " ≤ " << m_reorder_point
       << " → Order = " << (current_inventory <= m_reorder_point ? "LT×D = " : "0 (No order, IP > ROP)")
       << (order_qty > 0 ? std::to_string(m_lead_time) + "×" +
                               std::to_string(m_average_daily_demand) + " = " + std::to_string(order_qty)
                         : "");

    return ss.str();
}
