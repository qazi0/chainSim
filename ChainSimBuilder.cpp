
#include "ChainSimBuilder.h"

ChainSimBuilder::ChainSimBuilder(std::string_view simulationName) {
    m_root.m_simulation_name = simulationName;
}

ChainSimBuilder &ChainSimBuilder::simulation_length(std::size_t simulationLength) {
    m_root.m_simulation_length = simulationLength;
    return *this;
}

ChainSimBuilder &ChainSimBuilder::lead_time(std::size_t leadTime) {
    m_root.m_lead_time = leadTime;
    return *this;
}

ChainSimBuilder &ChainSimBuilder::average_demand(double demand) {
    m_root.m_current_demand = demand;
    return *this;
}

ChainSimBuilder &ChainSimBuilder::starting_inventory(std::size_t startingInventory) {
    m_root.m_starting_inventory = startingInventory;
    return *this;
}

ChainSim ChainSimBuilder::build() {
    for (const auto &col: m_root.m_records_columns)
        m_root.m_records[col] = vector<long>(m_root.m_simulation_length);

    return m_root;
}

ChainSimBuilder &ChainSimBuilder::logging_level(unsigned loggingLevel) {
    m_root.m_logging_level = loggingLevel;
    return *this;
}
