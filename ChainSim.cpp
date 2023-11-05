//
// Created by blakberrisigma on 03/11/2023.
//

#include "ChainSim.h"

ChainSim::ChainSim(unsigned simulationLength,
                       unsigned leadTime,
                       unsigned demand,
                       unsigned startingInventory) :
        m_simulation_length{simulationLength},
        m_lead_time{leadTime},
        m_current_demand{demand} {

    m_records_columns = {
            "inventory_quantity",
            "demand_quantity",
            "procurement_quantity",
            "purchase_quantity",
            "sale_quantity",
            "lost_sale_quantity"
    };

    for (const auto &col: m_records_columns)
        m_records[col] = vector<long>(m_simulation_length);

    initialize_simulation(startingInventory);
}

void ChainSim::initialize_simulation(unsigned startingInventory) {

    for (const auto &col: m_records_columns)
        std::fill(m_records[col].begin(), m_records[col].end(), 0LL);

    std::fill(m_records["demand_quantity"].begin(), m_records["demand_quantity"].end(), m_current_demand);
    m_records["inventory_quantity"][0] = startingInventory;
}

void ChainSim::simulate(const PurchaseMethod &purchase_method) {

    for (unsigned index = 1; index < m_simulation_length; ++index) {
        auto current_inventory = m_records["inventory_quantity"][index - 1];
        auto current_demand = m_records["demand_quantity"][index];
        auto current_procurement = m_records["procurement_quantity"][index];

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

        m_records["inventory_quantity"][index] = current_inventory;
        m_records["sale_quantity"][index] = sales;
        m_records["lost_sale_quantity"][index] = lost_sales;

        auto purchase_quantity = purchase_method.get_purchase(m_records, index);

        if (purchase_quantity > 0) {
            m_records["purchase_quantity"][index] = purchase_quantity;

            auto delivery_date = std::min(index + m_lead_time, m_simulation_length);
            m_records["procurement_quantity"][delivery_date] += purchase_quantity;
        }

    }

}

unordered_map<string, vector<long>> ChainSim::get_simulation_records() const {
    return m_records;
}
