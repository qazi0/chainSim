
#include "ChainSim.h"
#include <iomanip>

ChainSim::ChainSim() {

    m_records_columns = {
            "inventory_quantity",
            "demand_quantity",
            "procurement_quantity",
            "purchase_quantity",
            "sale_quantity",
            "lost_sale_quantity"
    };
}

void ChainSim::initialize_simulation() {
    for (const auto &col: m_records_columns)
        std::fill(m_records[col].begin(), m_records[col].end(), 0LL);

    std::fill(m_records["demand_quantity"].begin(), m_records["demand_quantity"].end(), m_current_demand);
    m_records["inventory_quantity"][0] = m_starting_inventory;

    m_logger = ChainLogger(m_logging_level);
    m_logger.info("Successfully initialized simulation.");
}

void ChainSim::simulate(const PurchaseMethod &purchase_method) {
    m_logger.info("Starting simulation ...\n");

    for (unsigned index = 1; index < m_simulation_length; ++index) {
        m_logger.info("Day# ", index);

        auto current_inventory = m_records["inventory_quantity"][index - 1];
        auto current_demand = m_records["demand_quantity"][index];
        auto current_procurement = m_records["procurement_quantity"][index];

        m_logger.info(std::setw(30), std::right,
                      "Starting inventory: ", current_inventory,
                      std::setw(25), std::left,
                      "\tCurrent Demand: ", current_demand,
                      std::setw(20), std::left,
                      "\tIncoming: ", current_procurement);

        // Procurement is incorporated at the beginning of the day/time slice.
        current_inventory += current_procurement;

        long sales{}, lost_sales{};

        if (current_inventory >= current_demand) {
            sales = current_demand;
            current_inventory -= current_demand;
        } else {
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

        m_records["inventory_quantity"][index] = current_inventory;
        m_records["sale_quantity"][index] = sales;
        m_records["lost_sale_quantity"][index] = lost_sales;

        auto purchase_quantity = purchase_method.get_purchase(m_records, index);

        if (purchase_quantity > 0) {
            m_records["purchase_quantity"][index] = purchase_quantity;

            auto delivery_date = std::min(index + m_lead_time, m_simulation_length);
            m_records["procurement_quantity"][delivery_date] += purchase_quantity;

            m_logger.info("Using Purchase policy: ", purchase_method.getName());
            m_logger.warn("Purchasing {", purchase_quantity, "} units, will arrive on day {", delivery_date, "}.");
        }

        m_logger.info("");

    }

}

unordered_map<string, vector<long>> ChainSim::get_simulation_records() const {
    return m_records;
}
