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

qz::ChainSimBuilder &qz::ChainSimBuilder::setAverageDemand(double averageDemand)
{
    if (averageDemand <= 0)
    {
        throw std::invalid_argument("Average demand must be positive");
    }
    m_average_demand = averageDemand;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setDemandStdDev(double stdDev)
{
    if (stdDev < 0)
    {
        throw std::invalid_argument("Standard deviation cannot be negative");
    }
    m_demand_stddev = stdDev;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setDeterministic(bool deterministic)
{
    m_deterministic = deterministic;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setSeed(unsigned seed)
{
    m_seed = seed;
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

qz::ChainSimBuilder &qz::ChainSimBuilder::setDemandDistribution(const QString &distribution)
{
    if (distribution != "fixed" &&
        distribution != "normal" &&
        distribution != "gamma" &&
        distribution != "poisson" &&
        distribution != "uniform")
    {
        throw std::invalid_argument("Invalid demand distribution");
    }
    m_demand_distribution = distribution;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setGammaParameters(double shape, double scale)
{
    if (shape <= 0 || scale <= 0)
    {
        throw std::invalid_argument("Gamma parameters must be positive");
    }
    m_gamma_shape = shape;
    m_gamma_scale = scale;
    return *this;
}

qz::ChainSimBuilder &qz::ChainSimBuilder::setUniformParameters(double min, double max)
{
    if (min >= max)
    {
        throw std::invalid_argument("Uniform min must be less than max");
    }
    m_uniform_min = min;
    m_uniform_max = max;
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
    if (m_average_demand <= 0)
    {
        throw std::invalid_argument("Average demand not set or invalid");
    }
}

std::unique_ptr<qz::ChainSim> qz::ChainSimBuilder::create()
{
    validateConfiguration();

    auto sim = std::unique_ptr<ChainSim>(new ChainSim);
    sim->m_simulation_name = m_simulation_name;
    sim->m_simulation_length = m_simulation_length;
    sim->m_lead_time = m_lead_time;

    if (m_deterministic)
    {
        sim->m_demandSampler = std::make_unique<FixedDemandSampler>(m_average_demand);
    }
    else if (m_demand_distribution == "normal")
    {
        sim->m_demandSampler = std::make_unique<NormalDemandSampler>(
            m_average_demand, m_demand_stddev, m_seed);
    }
    else if (m_demand_distribution == "gamma")
    {
        sim->m_demandSampler = std::make_unique<GammaDemandSampler>(
            m_gamma_shape, m_gamma_scale, m_seed);
    }
    else if (m_demand_distribution == "poisson")
    {
        sim->m_demandSampler = std::make_unique<PoissonDemandSampler>(
            m_average_demand, m_seed);
    }
    else if (m_demand_distribution == "uniform")
    {
        sim->m_demandSampler = std::make_unique<UniformDemandSampler>(
            m_uniform_min, m_uniform_max, m_seed);
    }

    sim->m_starting_inventory = m_starting_inventory;
    sim->m_logging_level = m_logging_level;

    for (const auto &col : sim->m_records_columns)
    {
        sim->m_records[col] = QVector<qint64>(m_simulation_length);
    }

    return sim;
}
