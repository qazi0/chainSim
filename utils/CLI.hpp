#ifndef CHAINSIM_CLI_HPP
#define CHAINSIM_CLI_HPP

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QCoreApplication>

namespace qz
{

    void parse_command_line_args(QCommandLineParser &parser, QCoreApplication &app)
    {
        parser.setApplicationDescription("ChainSim - Supply Chain Simulation Tool");
        parser.addHelpOption();
        parser.addVersionOption();

        // Server mode option
        QCommandLineOption serverOption(
            QStringList() << "s" << "server",
            "Run in server mode listening on port 47761");

        // Add options
        QCommandLineOption logLevelOption(
            "log_level",
            "Logging level (0-2)",
            "level",
            "0");

        QCommandLineOption simLengthOption(
            "simulation_length",
            "Length of the simulation",
            "days",
            "30");

        QCommandLineOption avgDemandOption(
            "average_demand",
            "Average of demand (per time unit)",
            "demand",
            "50.0");

        QCommandLineOption stdDemandOption(
            "std_demand",
            "Standard deviation of demand (per time unit)",
            "std",
            "10.0");

        QCommandLineOption avgLeadTimeOption(
            "average_lead_time",
            "Average lead time",
            "time",
            "5");

        QCommandLineOption purchasePeriodOption(
            "purchase_period",
            "Purchase period (applies to TPOP policy)",
            "period",
            "7");

        QCommandLineOption orderingCostOption(
            "ordering_cost",
            "Cost of placing an order (applies to EOQ policy)",
            "cost",
            "100.0");

        QCommandLineOption holdingCostOption(
            "holding_cost",
            "Annual holding cost rate (applies to EOQ policy)",
            "rate",
            "0.2");

        QCommandLineOption minLotSizeOption(
            "minimum_lot_size",
            "Minimum lot size (quantity procure-able from a supplier)",
            "size",
            "10");

        QCommandLineOption seedOption(
            "seed",
            "PRNG seed, for demand/lead time generators",
            "seed",
            "7");

        QCommandLineOption startingInventoryOption(
            "starting_inventory",
            "Starting inventory for the simulation",
            "inventory",
            "0");

        QCommandLineOption outputFileOption(
            "output_file",
            "Path/name of the output records file (.csv)",
            "file",
            "simulation_records.csv");

        QCommandLineOption policyOption(
            "policy",
            "Purchasing policy: (ROP | TPOP | EOQ)",
            "policy",
            "ROP");

        QCommandLineOption deterministicOption(
            "deterministic",
            "Simulate without random sampling, use fixed leadtime/demand/etc.");

        // Add all options to parser
        parser.addOption(serverOption);
        parser.addOption(logLevelOption);
        parser.addOption(simLengthOption);
        parser.addOption(avgDemandOption);
        parser.addOption(stdDemandOption);
        parser.addOption(avgLeadTimeOption);
        parser.addOption(purchasePeriodOption);
        parser.addOption(orderingCostOption);
        parser.addOption(holdingCostOption);
        parser.addOption(minLotSizeOption);
        parser.addOption(seedOption);
        parser.addOption(startingInventoryOption);
        parser.addOption(outputFileOption);
        parser.addOption(policyOption);
        parser.addOption(deterministicOption);

        // Process the command line arguments
        parser.process(app);

        // Only validate policy choice if not in server mode
        if (!parser.isSet(serverOption))
        {
            QString policy = parser.value(policyOption);
            if (policy != "ROP" && policy != "TPOP" && policy != "EOQ")
            {
                parser.showHelp(1);
            }
        }
    }
}

#endif // CHAINSIM_CLI_HPP
