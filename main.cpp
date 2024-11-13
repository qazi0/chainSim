#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include "ChainSimBuilder.h"
#include "purchase_policies/PurchaseROP.h"
#include "purchase_policies/PurchaseEOQ.h"
#include "purchase_policies/PurchaseTPOP.h"
#include "utils/CLI.hpp"

void print_simulation_config(const QCommandLineParser &parser, const PurchasePolicy &policy)
{
    if (parser.value("log_level").toUInt() < 1)
        return;

    QTextStream out(stdout);
    QString separator(80, '=');
    QString subseparator(80, '-');

    // Setup field widths for columns
    const int labelWidth = 30; // Width for parameter labels
    const int valueWidth = 20; // Width for values
    const int descWidth = 30;  // Width for descriptions

    // Header
    out << "\n"
        << separator << "\n";
    out << "ChainSim Configuration\n";
    out << subseparator << "\n\n";

    // Print column headers with consistent alignment
    out.setFieldAlignment(QTextStream::AlignLeft);
    out.setFieldWidth(labelWidth);
    out << "Parameter";
    out.setFieldWidth(valueWidth);
    out << "Value";
    out.setFieldWidth(descWidth);
    out << "Description";
    out << "\n"
        << subseparator << "\n";

    // Helper lambda for consistent row formatting
    auto printRow = [&](const QString &param, const QString &value, const QString &desc)
    {
        out.setFieldAlignment(QTextStream::AlignLeft);
        out.setFieldWidth(labelWidth);
        out << param;
        out.setFieldWidth(valueWidth);
        out << value;
        out.setFieldWidth(0); // Reset for description as it might be longer
        out << desc << "\n";
    };

    // General parameters
    printRow("Simulation Length",
             parser.value("simulation_length"),
             "Time periods");

    printRow("Average Demand",
             parser.value("average_demand"),
             "Units per period");

    printRow("Lead Time",
             parser.value("average_lead_time"),
             "Periods");

    printRow("Starting Inventory",
             parser.value("starting_inventory"),
             "Units");

    // Policy specific parameters
    out << "\nPolicy Configuration: " << policy.name() << "\n";
    out << subseparator << "\n";

    if (policy.name() == "EOQ")
    {
        printRow("Ordering Cost",
                 parser.value("ordering_cost"),
                 "Cost per order");

        printRow("Holding Cost Rate",
                 parser.value("holding_cost"),
                 "Annual rate");
    }
    else if (policy.name() == "TPOP")
    {
        printRow("Review Period",
                 parser.value("purchase_period"),
                 "Periods");
    }

    // Output Configuration
    out << "\nOutput Configuration\n";
    out << subseparator << "\n";

    printRow("Output File",
             parser.value("output_file"),
             "");

    printRow("Log Level",
             parser.value("log_level"),
             "");

    out << separator << "\n\n";
    out.flush(); // Ensure all output is written
}

std::unique_ptr<PurchasePolicy> create_policy(const QString &policy_name,
                                              unsigned lead_time,
                                              double avg_demand,
                                              const QCommandLineParser &parser)
{
    if (policy_name == "ROP")
    {
        return std::make_unique<PurchaseROP>(lead_time, avg_demand);
    }
    else if (policy_name == "EOQ")
    {
        double ordering_cost = parser.value("ordering_cost").toDouble();
        double holding_cost = parser.value("holding_cost").toDouble();
        return std::make_unique<PurchaseEOQ>(lead_time, avg_demand, ordering_cost, holding_cost);
    }
    else if (policy_name == "TPOP")
    {
        unsigned review_period = parser.value("purchase_period").toULongLong();
        return std::make_unique<PurchaseTPOP>(lead_time, avg_demand, review_period);
    }
    else
    {
        throw std::invalid_argument("Unsupported policy: " + policy_name.toStdString());
    }
}

void save_results(const qz::ChainSim::simulation_records_t &records,
                  const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw std::runtime_error("Could not open output file: " + filename.toStdString());
    }

    QTextStream out(&file);

    // Write header
    out << "Day,inventory_quantity,demand_quantity,procurement_quantity,"
        << "purchase_quantity,sale_quantity,lost_sale_quantity\n";

    // Write data
    for (size_t i = 0; i < records.values()[0].size(); ++i)
    {
        out << i << ","
            << records.value("inventory_quantity").at(i) << ","
            << records.value("demand_quantity").at(i) << ","
            << records.value("procurement_quantity").at(i) << ","
            << records.value("purchase_quantity").at(i) << ","
            << records.value("sale_quantity").at(i) << ","
            << records.value("lost_sale_quantity").at(i) << "\n";
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("ChainSim");
    QCoreApplication::setApplicationVersion("0.2");

    try
    {
        QCommandLineParser parser;
        qz::parse_command_line_args(parser, app);

        // Get simulation parameters
        auto log_level = parser.value("log_level").toUInt();
        auto simulation_length = parser.value("simulation_length").toULongLong();
        auto lead_time = parser.value("average_lead_time").toULongLong();
        auto demand = parser.value("average_demand").toDouble();
        auto starting_inventory = parser.value("starting_inventory").toULongLong();
        auto policy_name = parser.value("policy");
        auto output_file = parser.value("output_file");

        // Create appropriate policy
        auto policy = create_policy(policy_name, lead_time, demand, parser);

        // Print configuration if log level > 0
        print_simulation_config(parser, *policy);

        // Create and configure simulation
        auto chainSimulator = qz::ChainSimBuilder()
                                  .setSimulationName("ChainSim")
                                  .setSimulationLength(simulation_length)
                                  .setLeadTime(lead_time)
                                  .setAverageDemand(demand)
                                  .setStartingInventory(starting_inventory)
                                  .setLoggingLevel(log_level)
                                  .create();

        chainSimulator->initialize_simulation();

        // Run simulation
        chainSimulator->simulate(*policy);

        // Get and save results
        auto simulation_records = chainSimulator->get_simulation_records();
        save_results(simulation_records, output_file);

        return 0;
    }
    catch (const std::exception &e)
    {
        qCritical() << "Error:" << e.what();
        return 1;
    }
}
