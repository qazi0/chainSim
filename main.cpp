#include <iostream>
#include "ChainSim.h"
#include "PurchaseROP.h"

int main() {

    unsigned simulation_length = 365;
    unsigned lead_time = 8;
    unsigned demand = 50;
    unsigned starting_inventory = 400;

    ChainSim chainSimulator{simulation_length, lead_time, demand, starting_inventory};

    PurchaseROP rop(lead_time, demand);
    chainSimulator.simulate(rop);

    auto simulation_record = chainSimulator.get_simulation_records();


    return 0;
}
