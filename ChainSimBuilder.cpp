#include "ChainSimBuilder.h"
#include <stdexcept>

qz::ChainSimBuilder::ChainSimBuilder(std::string_view simulationName)
{
    if (simulationName.empty())
    {
        throw std::invalid_argument("Simulation name cannot be empty");
    }
    m_root.m_simulation_name = simulationName;

    // Set reasonable defaults
    m_root.m_simulation_length = 30;
    m_root.m_lead_time = 5;
    m_root.m_current_demand = 50.0;
    m_root.m_starting_inventory = 0;
    m_root.m_logging_level = 0;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::simulation_length(uint64_t simulationLength)
{
    if (simulationLength == 0)
    {
        throw std::invalid_argument("Simulation length must be greater than zero");
    }
    m_root.m_simulation_length = simulationLength;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::lead_time(uint64_t leadTime)
{
    if (leadTime == 0)
    {
        throw std::invalid_argument("Lead time must be greater than zero");
    }
    if (leadTime >= m_root.m_simulation_length)
    {
        throw std::invalid_argument("Lead time must be less than simulation length");
    }
    m_root.m_lead_time = leadTime;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::average_demand(double demand)
{
    if (demand <= 0)
    {
        throw std::invalid_argument("Average demand must be positive");
    }
    m_root.m_current_demand = demand;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::starting_inventory(uint64_t startingInventory)
{
    m_root.m_starting_inventory = startingInventory;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::logging_level(unsigned loggingLevel)
{
    if (loggingLevel > 2)
    {
        throw std::invalid_argument("Logging level must be between 0 and 2");
    }
    m_root.m_logging_level = loggingLevel;
    return *this;
}

qz::ChainSim qz::ChainSimBuilder::build()
{
    // Validate final configuration
    if (m_root.m_simulation_length == 0)
    {
        throw std::invalid_argument("Simulation length not set or invalid");
    }
    if (m_root.m_lead_time == 0)
    {
        throw std::invalid_argument("Lead time not set or invalid");
    }
    if (m_root.m_current_demand <= 0)
    {
        throw std::invalid_argument("Average demand not set or invalid");
    }

    // Initialize records vectors
    for (const auto &col : m_root.m_records_columns)
    {
        m_root.m_records[col] = std::vector<long>(m_root.m_simulation_length);
    }

    return m_root;
}
