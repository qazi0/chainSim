#include "ChainSimBuilder.h"
#include "purchase_policies/PurchaseROP.h"
#include "purchase_policies/PurchaseEOQ.h"
#include "purchase_policies/PurchaseTPOP.h"
#include "utils/CLI.hpp"
#include <memory>
#include <fstream>
#include <iomanip>

void print_simulation_config(const argparse::ArgumentParser &parser,
                             const PurchasePolicy &policy)
{
    if (parser.get<unsigned>("log_level") < 1)
        return;

    std::cout << "\n"
              << std::string(80, '=') << "\n";
    std::cout << "ChainSim Configuration\n";
    std::cout << std::string(80, '-') << "\n\n";

    // General parameters
    std::cout << std::left << std::setw(25) << "Parameter"
              << std::setw(15) << "Value" << "Description\n";
    std::cout << std::string(80, '-') << "\n";

    std::cout << std::left << std::setw(25) << "Simulation Length"
              << std::setw(15) << parser.get<uint64_t>("simulation_length")
              << "Time periods\n";

    std::cout << std::left << std::setw(25) << "Average Demand"
              << std::setw(15) << parser.get<double>("average_demand")
              << "Units per period\n";

    std::cout << std::left << std::setw(25) << "Lead Time"
              << std::setw(15) << parser.get<uint64_t>("average_lead_time")
              << "Periods\n";

    std::cout << std::left << std::setw(25) << "Starting Inventory"
              << std::setw(15) << parser.get<uint64_t>("starting_inventory")
              << "Units\n";

    // Policy specific parameters
    std::cout << "\nPolicy Configuration: " << policy.name() << "\n";
    std::cout << std::string(80, '-') << "\n";

    if (policy.name() == "EOQ")
    {
        std::cout << std::left << std::setw(25) << "Ordering Cost"
                  << std::setw(15) << parser.get<double>("ordering_cost")
                  << "Cost per order\n";
        std::cout << std::left << std::setw(25) << "Holding Cost Rate"
                  << std::setw(15) << parser.get<double>("holding_cost")
                  << "Annual rate\n";
    }
    else if (policy.name() == "TPOP")
    {
        std::cout << std::left << std::setw(25) << "Review Period"
                  << std::setw(15) << parser.get<uint64_t>("purchase_period")
                  << "Periods\n";
    }

    std::cout << "\nOutput Configuration\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::left << std::setw(25) << "Output File"
              << parser.get<std::string>("output_file") << "\n";
    std::cout << std::left << std::setw(25) << "Log Level"
              << parser.get<unsigned>("log_level") << "\n";

    std::cout << std::string(80, '=') << "\n\n";
}

std::unique_ptr<PurchasePolicy> create_policy(const std::string &policy_name,
                                              unsigned lead_time,
                                              double avg_demand,
                                              const argparse::ArgumentParser &parser)
{
    if (policy_name == "ROP")
    {
        return std::make_unique<PurchaseROP>(lead_time, avg_demand);
    }
    else if (policy_name == "EOQ")
    {
        double ordering_cost = parser.get<double>("ordering_cost");
        double holding_cost = parser.get<double>("holding_cost");
        return std::make_unique<PurchaseEOQ>(lead_time, avg_demand, ordering_cost, holding_cost);
    }
    else if (policy_name == "TPOP")
    {
        unsigned review_period = parser.get<uint64_t>("purchase_period");
        return std::make_unique<PurchaseTPOP>(lead_time, avg_demand, review_period);
    }
    else
    {
        throw std::invalid_argument("Unsupported policy: " + policy_name);
    }
}

void save_results(const qz::ChainSim::simulation_records_t &records,
                  const std::string &filename)
{
    std::ofstream outfile(filename);
    if (!outfile)
    {
        throw std::runtime_error("Could not open output file: " + filename);
    }

    // Write header
    outfile << "Day,inventory_quantity,demand_quantity,procurement_quantity,"
            << "purchase_quantity,sale_quantity,lost_sale_quantity\n";

    // Write data
    for (size_t i = 0; i < records.begin()->second.size(); ++i)
    {
        outfile << i << ","
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
    try
    {
        argparse::ArgumentParser parser("ChainSim", "0.2");
        qz::parse_command_line_args(parser, argc, argv);

        // Get simulation parameters
        auto log_level = parser.get<unsigned>("log_level");
        auto simulation_length = parser.get<uint64_t>("simulation_length");
        auto lead_time = parser.get<uint64_t>("average_lead_time");
        auto demand = parser.get<double>("average_demand");
        auto starting_inventory = parser.get<uint64_t>("starting_inventory");
        auto policy_name = parser.get<std::string>("policy");
        auto output_file = parser.get<std::string>("output_file");

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
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
