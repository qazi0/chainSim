#include "ChainSimBuilder.h"

qz::ChainSimBuilder::ChainSimBuilder(QObject *parent)
    : QObject(parent)
{
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setSimulationName(const QString &simulationName)
{
    if (simulationName.isEmpty())
    {
        throw std::invalid_argument("Simulation name cannot be empty");
    }
    m_simulation_name = simulationName;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setSimulationLength(quint64 simulationLength)
{
    if (simulationLength == 0)
    {
        throw std::invalid_argument("Simulation length must be greater than zero");
    }
    m_simulation_length = simulationLength;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setLeadTime(quint64 leadTime)
{
    if (leadTime == 0)
    {
        throw std::invalid_argument("Lead time must be greater than zero");
    }
    if (leadTime >= m_simulation_length)
    {
        throw std::invalid_argument("Lead time must be less than simulation length");
    }
    m_lead_time = leadTime;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setAverageDemand(double demand)
{
    if (demand <= 0)
    {
        throw std::invalid_argument("Average demand must be positive");
    }
    m_current_demand = demand;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setStartingInventory(quint64 startingInventory)
{
    m_starting_inventory = startingInventory;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setLoggingLevel(quint32 loggingLevel)
{
    if (loggingLevel > 2)
    {
        throw std::invalid_argument("Logging level must be between 0 and 2");
    }
    m_logging_level = loggingLevel;
    return *this;
}

void qz::ChainSimBuilder::validateConfiguration() const
{
    if (m_simulation_name.isEmpty())
    {
        throw std::invalid_argument("Simulation name not set");
    }
    if (m_simulation_length == 0)
    {
        throw std::invalid_argument("Simulation length not set or invalid");
    }
    if (m_lead_time == 0)
    {
        throw std::invalid_argument("Lead time not set or invalid");
    }
    if (m_current_demand <= 0)
    {
        throw std::invalid_argument("Average demand not set or invalid");
    }
}

std::unique_ptr<qz::ChainSim> qz::ChainSimBuilder::create()
{
    validateConfiguration();

    // Create new ChainSim instance
    auto sim = std::unique_ptr<ChainSim>(new ChainSim);

    // Configure the instance
    sim->m_simulation_name = m_simulation_name;
    sim->m_simulation_length = m_simulation_length;
    sim->m_lead_time = m_lead_time;
    sim->m_current_demand = m_current_demand;
    sim->m_starting_inventory = m_starting_inventory;
    sim->m_logging_level = m_logging_level;

    // Initialize records vectors
    for (const auto &col : sim->m_records_columns)
    {
        sim->m_records[col] = QVector<qint64>(m_simulation_length);
    }

    return sim;
}
