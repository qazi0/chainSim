//
// Created by blakberrisigma on 06/11/2023.
//

#include "PurchaseEOQ.h"

PurchaseEOQ::PurchaseEOQ(quint32 leadTime,
                         double avgDemand,
                         double orderingCost,
                         double holdingCostRate,
                         QObject *parent)
    : PurchasePolicy(parent), m_lead_time{leadTime}, m_average_daily_demand{avgDemand}, m_ordering_cost{orderingCost}, m_holding_cost_rate{holdingCostRate}
{
    validate_parameters();
    calculate_eoq();

    // Calculate reorder point (ROP = Lead Time Demand + Safety Stock)
    double safety_stock = std::ceil(m_average_daily_demand * m_lead_time * 0.5); // 50% safety factor
    m_reorder_point = static_cast<qint64>(m_average_daily_demand * m_lead_time + safety_stock);
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

qint64 PurchaseEOQ::get_purchase(const simulation_records_t &pastRecords,
                                 quint32 current_day) const
{
    auto current_inventory = pastRecords[QStringLiteral("inventory_quantity")][current_day];

    // Include incoming orders in the inventory position
    qint64 pipeline_inventory = 0;
    for (quint32 i = current_day + 1;
         i < qMin(current_day + m_lead_time + 1, quint32(pastRecords[QStringLiteral("procurement_quantity")].size()));
         ++i)
    {
        pipeline_inventory += pastRecords[QStringLiteral("procurement_quantity")][i];
    }

    qint64 inventory_position = current_inventory + pipeline_inventory;

    // Order if inventory position falls below reorder point
    if (inventory_position <= m_reorder_point)
    {
        return static_cast<qint64>(std::ceil(m_eoq));
    }

    return 0;
}

QString PurchaseEOQ::name() const
{
    return QStringLiteral("EOQ");
}

QString PurchaseEOQ::get_calculation_details(
    const simulation_records_t &pastRecords,
    quint32 current_day) const
{
    auto current_inventory = pastRecords[QStringLiteral("inventory_quantity")][current_day];
    qint64 pipeline_inventory = 0;

    for (quint32 i = current_day + 1;
         i < qMin(current_day + m_lead_time + 1, quint32(pastRecords[QStringLiteral("procurement_quantity")].size()));
         ++i)
    {
        pipeline_inventory += pastRecords[QStringLiteral("procurement_quantity")][i];
    }

    qint64 inventory_position = current_inventory + pipeline_inventory;
    double annual_demand = m_average_daily_demand * 365.0;

    QString details;
    QTextStream ss(&details);

    ss << "EOQ = sqrt((2×D×S)/H) = sqrt((2×" << annual_demand << "×"
       << m_ordering_cost << ")/" << m_holding_cost_rate << ") = " << m_eoq << "\n"
       << "ROP = LT×D + SS = " << m_lead_time << "×" << m_average_daily_demand
       << " + " << (m_reorder_point - m_lead_time * m_average_daily_demand)
       << " = " << m_reorder_point << "\n\t  "
       << "INV = I + P = " << current_inventory << " + " << pipeline_inventory
       << " = " << inventory_position << " ≤ " << m_reorder_point
       << " → Order = " << (inventory_position <= m_reorder_point ? QString::number(static_cast<qint64>(std::ceil(m_eoq))) : QStringLiteral("0 (No order, IP > ROP)"));

    return details;
}
