#include "ChainSimBuilder.h"
#include "purchase_policies/PurchaseROP.h"
#include "utils/CLI.hpp"

int main(int argc, char *argv[]) {
    argparse::ArgumentParser parser("ChainSim", "0.1");
    qz::parse_command_line_args(parser, argc, argv);

    auto log_level = parser.get<unsigned>("log_level");
    auto simulationLength = parser.get<uint64_t>("simulation_length");
    auto leadTime = parser.get<uint64_t>("average_lead_time");
    auto demand = parser.get<double>("average_demand");
    auto startingInventory = parser.get<uint64_t>("starting_inventory");

    qz::ChainSim chainSimulator = (
            qz::ChainSimBuilder("ChainSim")
                    .simulation_length(simulationLength)
                    .lead_time(leadTime)
                    .average_demand(demand)
                    .starting_inventory(startingInventory)
                    .logging_level(log_level)
                    .build()
    );

    chainSimulator.initialize_simulation();

    PurchaseROP rop(leadTime, demand);
    chainSimulator.simulate(rop);

    auto simulation_record = chainSimulator.get_simulation_records();


    return 0;
}
