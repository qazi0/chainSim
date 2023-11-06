
#include "ChainSimBuilder.h"

qz::ChainSimBuilder::ChainSimBuilder(std::string_view simulationName) {
    m_root.m_simulation_name = simulationName;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::simulation_length(uint64_t simulationLength) {
    m_root.m_simulation_length = simulationLength;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::lead_time(uint64_t leadTime) {
    m_root.m_lead_time = leadTime;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::average_demand(double demand) {
    m_root.m_current_demand = demand;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::starting_inventory(uint64_t startingInventory) {
    m_root.m_starting_inventory = startingInventory;
    return *this;
}

qz::ChainSim qz::ChainSimBuilder::build() {
    for (const auto &col: m_root.m_records_columns)
        m_root.m_records[col] = std::vector<long>(m_root.m_simulation_length);

    return m_root;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::logging_level(unsigned loggingLevel) {
    m_root.m_logging_level = loggingLevel;
    return *this;
}
