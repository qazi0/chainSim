#include "ChainSim.h"

qz::ChainSim::ChainSim() : QObject()
{
    m_records_columns = {
                         QStringLiteral("inventory_quantity"),
                         QStringLiteral("demand_quantity"),
                         QStringLiteral("procurement_quantity"),
                         QStringLiteral("purchase_quantity"),
                         QStringLiteral("sale_quantity"),
                         QStringLiteral("lost_sale_quantity")};
}

void qz::ChainSim::initialize_simulation()
{
    for (const auto &col : m_records_columns)
    {
        m_records[col].fill(0, m_simulation_length);
    }

    m_records[QStringLiteral("demand_quantity")].fill(m_current_demand, m_simulation_length);
    m_records[QStringLiteral("inventory_quantity")][0] = m_starting_inventory;
    m_current_day = 1; // Reset current day

    m_logger = ChainLogger(m_logging_level);
    m_logger.info("Successfully initialized simulation.");

    Q_EMIT simulationStarted();
}

void qz::ChainSim::simulate(const PurchasePolicy &purchasePolicy)
{
    m_logger.info(QStringLiteral("Starting simulation {{%1}} ...\n").arg(m_simulation_name));

    while (m_current_day < m_simulation_length)
    {
        simulate_day(purchasePolicy, m_current_day);
        Q_EMIT daySimulated(m_current_day);
        m_current_day++;
    }

    Q_EMIT simulationFinished();
}

void qz::ChainSim::simulate_days(const PurchasePolicy &purchasePolicy, quint64 days)
{
    if (m_current_day + days > m_simulation_length)
    {
        QString error = QStringLiteral("Requested simulation days exceed simulation length");
        Q_EMIT errorOccurred(error);
        throw std::runtime_error(error.toStdString());
    }

    quint64 end_day = m_current_day + days;
    while (m_current_day < end_day)
    {
        simulate_day(purchasePolicy, m_current_day);
        Q_EMIT daySimulated(m_current_day);
        m_current_day++;
    }
}

void qz::ChainSim::simulate_day(const PurchasePolicy &purchasePolicy, quint64 day)
{
    m_logger.info(QStringLiteral("Day# %1").arg(day));

    auto current_inventory = m_records[QStringLiteral("inventory_quantity")][day - 1];
    auto current_demand = m_records[QStringLiteral("demand_quantity")][day];
    auto current_procurement = m_records[QStringLiteral("procurement_quantity")][day];

    m_logger.info(QString("%1%2%3%4%5%6")
                      .arg(QString(10, ' ') + QString("Starting inventory: "))
                      .arg(current_inventory)
                      .arg(QString(15, ' ') + QString("Current Demand: "))
                      .arg(current_demand)
                      .arg(QString(10, ' ') + QString("Incoming: "))
                      .arg(current_procurement));

    // Procurement is incorporated at the beginning of the day/time slice.
    current_inventory += current_procurement;

    qint64 sales{}, lost_sales{};

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

    m_logger.info(QString("%1%2%3%4%5%6")
                      .arg(QString(10, ' ') + QString("Sales: "))
                      .arg(sales)
                      .arg(QString(15, ' ') + QString("Lost Sales: "))
                      .arg(lost_sales)
                      .arg(QString(10, ' ') + QString("Ending Inventory: "))
                      .arg(current_inventory));

    m_records[QStringLiteral("inventory_quantity")][day] = current_inventory;
    m_records[QStringLiteral("sale_quantity")][day] = sales;
    m_records[QStringLiteral("lost_sale_quantity")][day] = lost_sales;

    auto purchase_quantity = purchasePolicy.get_purchase(m_records, day);

    if (purchase_quantity > 0)
    {
        m_records[QStringLiteral("purchase_quantity")][day] = purchase_quantity;

        auto delivery_date = qMin(day + m_lead_time, m_simulation_length - 1);
        m_records[QStringLiteral("procurement_quantity")][delivery_date] += purchase_quantity;

        m_logger.info(QStringLiteral("Using Purchase policy: %1").arg(purchasePolicy.name()));
        m_logger.debug(QStringLiteral("Policy calculation: %1").arg(purchasePolicy.get_calculation_details(m_records, day)));
        m_logger.warn(QStringLiteral("Purchasing {%1} units, will arrive on day {%2}.")
                          .arg(purchase_quantity)
                          .arg(delivery_date));
    }

    m_logger.info(QString());
}

qz::ChainSim::simulation_records_t qz::ChainSim::get_simulation_records() const
{
    return m_records;
}
