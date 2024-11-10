#include "PurchaseTPOP.h"

PurchaseTPOP::PurchaseTPOP(unsigned leadTime, double avgDemand, unsigned reviewPeriod)
    : m_lead_time{leadTime}, m_average_daily_demand{avgDemand}, m_review_period{reviewPeriod}
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

bool PurchaseTPOP::is_review_day(unsigned day) const
{
    return day % m_review_period == 0;
}

long PurchaseTPOP::get_purchase(const simulation_records_t &pastRecords,
                                unsigned current_day) const
{
    if (!is_review_day(current_day))
    {
        return 0;
    }

    auto current_inventory = pastRecords.at("inventory_quantity")[current_day];

    // Calculate pipeline inventory (orders already placed but not yet received)
    long pipeline_inventory = 0;
    for (unsigned i = current_day + 1; i < std::min(current_day + m_lead_time + 1,
                                                    static_cast<unsigned>(pastRecords.at("procurement_quantity").size()));
         ++i)
    {
        pipeline_inventory += pastRecords.at("procurement_quantity")[i];
    }

    long inventory_position = current_inventory + pipeline_inventory;
    long order_quantity = static_cast<long>(std::ceil(m_target_level - inventory_position));

    return std::max(0L, order_quantity);
}

std::string PurchaseTPOP::name() const
{
    return "TPOP";
}

std::string PurchaseTPOP::get_calculation_details(
    const simulation_records_t &pastRecords,
    unsigned current_day) const
{

    if (!is_review_day(current_day))
    {
        return "Not a review day (Day " + std::to_string(current_day) +
               " % " + std::to_string(m_review_period) + " ≠ 0)";
    }

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
    double protection_interval = m_review_period + m_lead_time;
    double expected_demand = m_average_daily_demand * protection_interval;
    double safety_stock = std::ceil(m_average_daily_demand * std::sqrt(protection_interval));

    std::stringstream ss;
    ss << "Protection Interval = R + LT = " << m_review_period << " + "
       << m_lead_time << " = " << protection_interval << "\n"
       << "Target = D×(R+LT) + SS = " << m_average_daily_demand << "×"
       << protection_interval << " + " << safety_stock << " = " << m_target_level << "\n\t  "
       << "INV = I + P = " << current_inventory << " + " << pipeline_inventory
       << " = " << inventory_position << "\n\t"
       << "Order = max(0, Target - IP) = max(0, " << m_target_level << " - "
       << inventory_position << ") = "
       << std::max(0L, static_cast<long>(std::ceil(m_target_level - inventory_position)));

    return ss.str();
}
