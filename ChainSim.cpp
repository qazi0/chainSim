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

    // Sample demand for each day
    for (int i = 0; i < m_simulation_length; ++i)
    {
        m_records[QStringLiteral("demand_quantity")][i] =
            static_cast<qint64>(m_demandSampler->sample());
    }

    m_records[QStringLiteral("inventory_quantity")][0] = m_starting_inventory;
    m_current_day = 1; // Reset current day

    m_logger = ChainLogger(m_logging_level);
    m_logger.info("Successfully initialized simulation.");

    Q_EMIT simulationStarted();
}

void qz::ChainSim::simulate(const PurchasePolicy &purchasePolicy)
{
    m_logger.info(QString("Starting simulation {{%1}} ...").arg(m_simulation_name));

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
    // Setup field widths for consistent formatting
    const int leftMargin = 3;    // Left margin space
    const int labelWidth = 22;   // Width for labels
    const int valueWidth = 6;    // Width for values
    const int columnSpacing = 4; // Space between columns

    // Helper lambda for formatting a row with three columns
    auto formatRow = [=](const QString &label1, qint64 value1,
                         const QString &label2, qint64 value2,
                         const QString &label3, qint64 value3) -> QString
    {
        QString row;
        QTextStream ts(&row);

        // First column
        ts << QString(leftMargin, ' ')
           << qSetFieldWidth(labelWidth) << Qt::left << label1
           << qSetFieldWidth(valueWidth) << value1;

        // Second column
        ts << QString(columnSpacing, ' ')
           << qSetFieldWidth(labelWidth) << Qt::left << label2
           << qSetFieldWidth(valueWidth) << value2;

        // Third column
        ts << QString(columnSpacing, ' ')
           << qSetFieldWidth(labelWidth) << Qt::left << label3
           << qSetFieldWidth(valueWidth) << value3;

        return row;
    };

    // Day header
    m_logger.info(QString(80, '-'));
    m_logger.info(QString("%1Day #%2").arg(QString(leftMargin, ' ')).arg(day, 4, 10, QLatin1Char('0')));

    auto current_inventory = m_records[QStringLiteral("inventory_quantity")][day - 1];
    auto current_demand = m_records[QStringLiteral("demand_quantity")][day];
    auto current_procurement = m_records[QStringLiteral("procurement_quantity")][day];

    // First line: Initial state
    m_logger.info(formatRow(
        QStringLiteral("Starting inventory:"), current_inventory,
        QStringLiteral("Current demand:"), current_demand,
        QStringLiteral("Incoming procurement:"), current_procurement));

    // Process day's transactions
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

    // Second line: Transaction results
    m_logger.info(formatRow(
        QStringLiteral("Sales completed:"), sales,
        QStringLiteral("Lost sales:"), lost_sales,
        QStringLiteral("Ending inventory:"), current_inventory));

    // Update records
    m_records[QStringLiteral("inventory_quantity")][day] = current_inventory;
    m_records[QStringLiteral("sale_quantity")][day] = sales;
    m_records[QStringLiteral("lost_sale_quantity")][day] = lost_sales;

    // Purchase decision
    auto purchase_quantity = purchasePolicy.get_purchase(m_records, day);

    if (purchase_quantity > 0)
    {
        m_records[QStringLiteral("purchase_quantity")][day] = purchase_quantity;
        auto delivery_date = qMin(day + m_lead_time, m_simulation_length - 1);
        m_records[QStringLiteral("procurement_quantity")][delivery_date] += purchase_quantity;

        // Lines 3-5: Calculation details
        m_logger.info(QString("%1Calculation Details:").arg(QString(leftMargin, ' ')));
        QString details = purchasePolicy.get_calculation_details(m_records, day);
        for (const QString &line : details.split('\n'))
        {
            m_logger.info(QString("%1%2").arg(QString(leftMargin + 2, ' ')).arg(line));
        }
    }

    m_logger.info(QString()); // Empty line between days
}

qz::ChainSim::simulation_records_t qz::ChainSim::get_simulation_records() const
{
    return m_records;
}
