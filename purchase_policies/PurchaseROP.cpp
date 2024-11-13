//
// Created by blakberrisigma on 03/11/2023.
//

#include "PurchaseROP.h"

PurchaseROP::PurchaseROP(quint32 leadTime, double avgDemand, QObject* parent)
    : PurchasePolicy(parent), m_lead_time{leadTime}, m_average_daily_demand{avgDemand}
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
    m_reorder_point = static_cast<qint64>(m_average_daily_demand * leadTime + m_safety_stock);
}

qint64 PurchaseROP::get_purchase(const simulation_records_t &pastRecords,
                                 quint32 current_day) const
{
    auto current_inventory = pastRecords[QStringLiteral("inventory_quantity")][current_day];
    qint64 reorder_quantity{0};
    if (current_inventory <= m_reorder_point)
        reorder_quantity = std::ceil(m_average_daily_demand * m_lead_time);

    return reorder_quantity;
}

QString PurchaseROP::name() const
{
    return QStringLiteral("ROP/CR");
}

QString PurchaseROP::get_calculation_details(
    const simulation_records_t &pastRecords,
    quint32 current_day) const
{

    auto current_inventory = pastRecords[QStringLiteral("inventory_quantity")][current_day];
    QString details;
    QTextStream ss(&details);

    auto order_qty = (current_inventory <= m_reorder_point) ? std::ceil(m_average_daily_demand * m_lead_time) : 0;

    ss << "ROP = LT×D + SS = " << m_lead_time << "×" << m_average_daily_demand
       << " + " << m_safety_stock << " = " << m_reorder_point << "\n"
       << "INV = " << current_inventory << " ≤ " << m_reorder_point
       << " → Order = " << (current_inventory <= m_reorder_point ? "LT×D = " : "0 (No order, IP > ROP)")
       << (order_qty > 0 ? QString("%1×%2 = %3")
                               .arg(m_lead_time)
                               .arg(m_average_daily_demand)
                               .arg(order_qty)
                         : QString());

    return details;
}
