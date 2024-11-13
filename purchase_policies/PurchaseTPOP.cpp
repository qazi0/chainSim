#include "PurchaseTPOP.h"

PurchaseTPOP::PurchaseTPOP(quint32 leadTime, double avgDemand, quint32 reviewPeriod, QObject* parent)
    : PurchasePolicy(parent), m_lead_time{leadTime}, m_average_daily_demand{avgDemand}, m_review_period{reviewPeriod}
{
    validate_parameters();
    calculate_target_level();
}

void PurchaseTPOP::validate_parameters() const
{
    if (m_lead_time == 0)
    {
        throw std::invalid_argument("Lead time must be greater than zero");
    }
    if (m_average_daily_demand <= 0)
    {
        throw std::invalid_argument("Average demand must be positive");
    }
    if (m_review_period == 0)
    {
        throw std::invalid_argument("Review period must be greater than zero");
    }
}

void PurchaseTPOP::calculate_target_level()
{
    // Target level = Expected demand during protection interval + Safety stock
    // Protection interval = Review period + Lead time
    double protection_interval = m_review_period + m_lead_time;
    double expected_demand = m_average_daily_demand * protection_interval;
    double safety_stock = std::ceil(m_average_daily_demand * std::sqrt(protection_interval));

    m_target_level = expected_demand + safety_stock;
}

bool PurchaseTPOP::is_review_day(quint32 day) const
{
    return day % m_review_period == 0;
}

qint64 PurchaseTPOP::get_purchase(const simulation_records_t &pastRecords,
                                  quint32 current_day) const
{
    if (!is_review_day(current_day))
    {
        return 0;
    }

    auto current_inventory = pastRecords[QStringLiteral("inventory_quantity")][current_day];

    // Calculate pipeline inventory (orders already placed but not yet received)
    qint64 pipeline_inventory = 0;
    for (quint32 i = current_day + 1;
         i < qMin(current_day + m_lead_time + 1, quint32(pastRecords[QStringLiteral("procurement_quantity")].size()));
         ++i)
    {
        pipeline_inventory += pastRecords[QStringLiteral("procurement_quantity")][i];
    }

    qint64 inventory_position = current_inventory + pipeline_inventory;
    qint64 order_quantity = static_cast<qint64>(std::ceil(m_target_level - inventory_position));

    return qMax(qint64{0}, order_quantity);
}

QString PurchaseTPOP::name() const
{
    return QStringLiteral("TPOP");
}

QString PurchaseTPOP::get_calculation_details(
    const simulation_records_t &pastRecords,
    quint32 current_day) const
{
    if (!is_review_day(current_day))
    {
        return QStringLiteral("Not a review day (Day %1 % %2 ≠ 0)")
            .arg(current_day)
            .arg(m_review_period);
    }

    auto current_inventory = pastRecords[QStringLiteral("inventory_quantity")][current_day];
    qint64 pipeline_inventory = 0;

    for (quint32 i = current_day + 1;
         i < qMin(current_day + m_lead_time + 1, quint32(pastRecords[QStringLiteral("procurement_quantity")].size()));
         ++i)
    {
        pipeline_inventory += pastRecords[QStringLiteral("procurement_quantity")][i];
    }

    qint64 inventory_position = current_inventory + pipeline_inventory;
    double protection_interval = m_review_period + m_lead_time;
    double expected_demand = m_average_daily_demand * protection_interval;
    double safety_stock = std::ceil(m_average_daily_demand * std::sqrt(protection_interval));

    QString details;
    QTextStream ss(&details);

    ss << "Protection Interval = R + LT = " << m_review_period << " + "
       << m_lead_time << " = " << protection_interval << "\n"
       << "Target = D×(R+LT) + SS = " << m_average_daily_demand << "×"
       << protection_interval << " + " << safety_stock << " = " << m_target_level << "\n"
       << "INV = I + P = " << current_inventory << " + " << pipeline_inventory
       << " = " << inventory_position << "\n\t"
       << "Order = max(0, Target - IP) = max(0, " << m_target_level << " - "
       << inventory_position << ") = "
       << qMax(qint64{0}, static_cast<qint64>(std::ceil(m_target_level - inventory_position)));

    return details;
}
