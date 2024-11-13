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

    out << "\n"
        << separator << "\n";
    out << "ChainSim Configuration\n";
    out << subseparator << "\n\n";

    // General parameters
    out.setFieldWidth(25);
    out << "Parameter";
    out.setFieldWidth(15);
    out << "Value";
    out.setFieldWidth(0);
    out << "Description\n";
    out << subseparator << "\n";

    out.setFieldAlignment(QTextStream::AlignLeft);

    out.setFieldWidth(25);
    out << "Simulation Length";
    out.setFieldWidth(15);
    out << parser.value("simulation_length");
    out.setFieldWidth(0);
    out << "Time periods\n";

    out.setFieldWidth(25);
    out << "Average Demand";
    out.setFieldWidth(15);
    out << parser.value("average_demand");
    out.setFieldWidth(0);
    out << "Units per period\n";

    out.setFieldWidth(25);
    out << "Lead Time";
    out.setFieldWidth(15);
    out << parser.value("average_lead_time");
    out.setFieldWidth(0);
    out << "Periods\n";

    out.setFieldWidth(25);
    out << "Starting Inventory";
    out.setFieldWidth(15);
    out << parser.value("starting_inventory");
    out.setFieldWidth(0);
    out << "Units\n";

    // Policy specific parameters
    out << "\nPolicy Configuration: " << QString::fromStdString(policy.name()) << "\n";
    out << subseparator << "\n";

    if (policy.name() == "EOQ")
    {
        out.setFieldWidth(25);
        out << "Ordering Cost";
        out.setFieldWidth(15);
        out << parser.value("ordering_cost");
        out.setFieldWidth(0);
        out << "Cost per order\n";

        out.setFieldWidth(25);
        out << "Holding Cost Rate";
        out.setFieldWidth(15);
        out << parser.value("holding_cost");
        out.setFieldWidth(0);
        out << "Annual rate\n";
    }
    else if (policy.name() == "TPOP")
    {
        out.setFieldWidth(25);
        out << "Review Period";
        out.setFieldWidth(15);
        out << parser.value("purchase_period");
        out.setFieldWidth(0);
        out << "Periods\n";
    }

    out << "\nOutput Configuration\n";
    out << subseparator << "\n";

    out.setFieldWidth(25);
    out << "Output File";
    out.setFieldWidth(0);
    out << parser.value("output_file") << "\n";

    out.setFieldWidth(25);
    out << "Log Level";
    out.setFieldWidth(0);
    out << parser.value("log_level") << "\n";

    out << separator << "\n\n";
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
    for (size_t i = 0; i < records.begin()->second.size(); ++i)
    {
        out << i << ","
            << records.at("inventory_quantity")[i] << ","
            << records.at("demand_quantity")[i] << ","
            << records.at("procurement_quantity")[i] << ","
            << records.at("purchase_quantity")[i] << ","
            << records.at("sale_quantity")[i] << ","
            << records.at("lost_sale_quantity")[i] << "\n";
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
        qz::ChainSim chainSimulator = (qz::ChainSimBuilder("ChainSim")
                                           .simulation_length(simulation_length)
                                           .lead_time(lead_time)
                                           .average_demand(demand)
                                           .starting_inventory(starting_inventory)
                                           .logging_level(log_level)
                                           .build());

        chainSimulator.initialize_simulation();

        // Run simulation
        chainSimulator.simulate(*policy);

        // Get and save results
        auto simulation_records = chainSimulator.get_simulation_records();
        save_results(simulation_records, output_file);

        return 0;
    }
    catch (const std::exception &e)
    {
        qCritical() << "Error:" << e.what();
        return 1;
    }
}
