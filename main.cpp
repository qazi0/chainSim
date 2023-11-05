#include "ChainSimBuilder.h"
#include "PurchaseROP.h"

int main() {

    unsigned simulationLength = 365;
    unsigned leadTime = 8;
    unsigned demand = 50;
    unsigned startingInventory = 400;

    ChainSim chainSimulator = (
            ChainSimBuilder("ChainSim")
                    .simulation_length(simulationLength)
                    .lead_time(leadTime)
                    .average_demand(demand)
                    .starting_inventory(startingInventory)
                    .logging_level(1)
                    .build()
    );

    chainSimulator.initialize_simulation();

    PurchaseROP rop(leadTime, demand);
    chainSimulator.simulate(rop);

    auto simulation_record = chainSimulator.get_simulation_records();


    return 0;
}
