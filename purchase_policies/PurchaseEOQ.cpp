//
// Created by blakberrisigma on 06/11/2023.
//

#include "PurchaseEOQ.h"

PurchaseEOQ::PurchaseEOQ(unsigned leadTime, double avgDemand,
                         double orderingCost, double holdingCostRate)
    : m_lead_time{leadTime}, m_average_daily_demand{avgDemand}, m_ordering_cost{orderingCost}, m_holding_cost_rate{holdingCostRate}
{

    validate_parameters();
    calculate_eoq();

    // Calculate reorder point (ROP = Lead Time Demand + Safety Stock)
    double safety_stock = std::ceil(m_average_daily_demand * m_lead_time * 0.5); // 50% safety factor
    m_reorder_point = m_average_daily_demand * m_lead_time + safety_stock;
}

void PurchaseEOQ::validate_parameters() const
{
    if (m_lead_time == 0)
    {
        throw std::invalid_argument("Lead time must be greater than zero");
    }
    if (m_average_daily_demand <= 0)
    {
        throw std::invalid_argument("Average demand must be positive");
    }
    if (m_ordering_cost <= 0)
    {
        throw std::invalid_argument("Ordering cost must be positive");
    }
    if (m_holding_cost_rate <= 0)
    {
        throw std::invalid_argument("Holding cost rate must be positive");
    }
}

void PurchaseEOQ::calculate_eoq()
{
    // EOQ formula: sqrt((2 * annual demand * ordering cost) / holding cost)
    double annual_demand = m_average_daily_demand * 365.0;
    m_eoq = std::sqrt((2.0 * annual_demand * m_ordering_cost) / m_holding_cost_rate);
}

long PurchaseEOQ::get_purchase(const simulation_records_t &pastRecords,
                               unsigned current_day) const
{
    auto current_inventory = pastRecords.at("inventory_quantity")[current_day];

    // Include incoming orders in the inventory position
    long pipeline_inventory = 0;
    for (unsigned i = current_day + 1; i < std::min(current_day + m_lead_time + 1,
                                                    static_cast<unsigned>(pastRecords.at("procurement_quantity").size()));
         ++i)
    {
        pipeline_inventory += pastRecords.at("procurement_quantity")[i];
    }

    long inventory_position = current_inventory + pipeline_inventory;

    // Order if inventory position falls below reorder point
    if (inventory_position <= m_reorder_point)
    {
        return static_cast<long>(std::ceil(m_eoq));
    }

    return 0;
}

std::string PurchaseEOQ::name() const
{
    return "EOQ";
}

std::string PurchaseEOQ::get_calculation_details(
    const simulation_records_t &pastRecords,
    unsigned current_day) const
{

    auto current_inventory = pastRecords.at("inventory_quantity")[current_day];
    long pipeline_inventory = 0;

    for (unsigned i = current_day + 1;
         i < std::min(current_day + m_lead_time + 1,
                      static_cast<unsigned>(pastRecords.at("procurement_quantity").size()));
         ++i)
    {
        pipeline_inventory += pastRecords.at("procurement_quantity")[i];
    }

    long inventory_position = current_inventory + pipeline_inventory;
    double annual_demand = m_average_daily_demand * 365.0;

    std::stringstream ss;
    ss << "EOQ = sqrt((2×D×S)/H) = sqrt((2×" << annual_demand << "×"
       << m_ordering_cost << ")/" << m_holding_cost_rate << ") = " << m_eoq << "\n"
       << "ROP = LT×D + SS = " << m_lead_time << "×" << m_average_daily_demand
       << " + " << (m_reorder_point - m_lead_time * m_average_daily_demand)
       << " = " << m_reorder_point << "\n\t  "
       << "INV = I + P = " << current_inventory << " + " << pipeline_inventory
       << " = " << inventory_position << " ≤ " << m_reorder_point
       << " → Order = " << (inventory_position <= m_reorder_point ? std::to_string(static_cast<long>(std::ceil(m_eoq))) : "0 (No order, IP > ROP)");

    return ss.str();
}
