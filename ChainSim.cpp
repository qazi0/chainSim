#include "ChainSim.h"
#include <iomanip>

qz::ChainSim::ChainSim()
{

    m_records_columns = {
                         "inventory_quantity",
                         "demand_quantity",
                         "procurement_quantity",
                         "purchase_quantity",
                         "sale_quantity",
                         "lost_sale_quantity"};
}

void qz::ChainSim::initialize_simulation()
{
    for (const auto &col : m_records_columns)
        std::fill(m_records[col].begin(), m_records[col].end(), 0LL);

    std::fill(m_records["demand_quantity"].begin(), m_records["demand_quantity"].end(), m_current_demand);
    m_records["inventory_quantity"][0] = m_starting_inventory;
    m_current_day = 1; // Reset current day

    m_logger = ChainLogger(m_logging_level);
    m_logger.info("Successfully initialized simulation.");
}

void qz::ChainSim::simulate(const PurchasePolicy &purchasePolicy)
{
    m_logger.info("Starting simulation {{", m_simulation_name, "}} ...\n");

    while (m_current_day < m_simulation_length)
    {
        simulate_day(purchasePolicy, m_current_day);
        m_current_day++;
    }
}

void qz::ChainSim::simulate_days(const PurchasePolicy &purchasePolicy, uint64_t days)
{
    if (m_current_day + days > m_simulation_length)
    {
        throw std::runtime_error("Requested simulation days exceed simulation length");
    }

    uint64_t end_day = m_current_day + days;
    while (m_current_day < end_day)
    {
        simulate_day(purchasePolicy, m_current_day);
        m_current_day++;
    }
}

void qz::ChainSim::simulate_day(const PurchasePolicy &purchasePolicy, uint64_t day)
{
    m_logger.info("Day# ", day);

    auto current_inventory = m_records["inventory_quantity"][day - 1];
    auto current_demand = m_records["demand_quantity"][day];
    auto current_procurement = m_records["procurement_quantity"][day];

    m_logger.info(std::setw(30), std::right,
                  "Starting inventory: ", current_inventory,
                  std::setw(25), std::left,
                  "\tCurrent Demand: ", current_demand,
                  std::setw(20), std::left,
                  "\tIncoming: ", current_procurement);

    // Procurement is incorporated at the beginning of the day/time slice.
    current_inventory += current_procurement;

    long sales{}, lost_sales{};

    if (current_inventory >= current_demand)
    {
        sales = current_demand;
        current_inventory -= current_demand;
    }
    else
    {
        sales = current_inventory;
        lost_sales = current_demand - current_inventory;
        current_inventory = 0;
    }

    m_logger.info(std::setw(30), std::right,
                  "Sales: ", sales,
                  std::setw(25), std::left,
                  "\tLost Sales: ", lost_sales,
                  std::setw(20), std::left,
                  "\tEnding Inventory: ", current_inventory);

    m_records["inventory_quantity"][day] = current_inventory;
    m_records["sale_quantity"][day] = sales;
    m_records["lost_sale_quantity"][day] = lost_sales;

    auto purchase_quantity = purchasePolicy.get_purchase(m_records, day);

    if (purchase_quantity > 0)
    {
        m_records["purchase_quantity"][day] = purchase_quantity;

        auto delivery_date = std::min(day + m_lead_time, m_simulation_length);
        m_records["procurement_quantity"][delivery_date] += purchase_quantity;

        m_logger.info("Using Purchase policy: ", purchasePolicy.name());
        m_logger.debug("Policy calculation: ", purchasePolicy.get_calculation_details(m_records, day));
        m_logger.warn("Purchasing {", purchase_quantity, "} units, will arrive on day {", delivery_date, "}.");
    }

    m_logger.info("");
}

qz::ChainSim::simulation_records_t qz::ChainSim::get_simulation_records() const
{
    return m_records;
}
