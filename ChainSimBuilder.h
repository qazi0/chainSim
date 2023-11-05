
#ifndef CHAINSIM_CHAINSIMBUILDER_H
#define CHAINSIM_CHAINSIMBUILDER_H

#include "ChainSim.h"

class ChainSimBuilder {
public:
    explicit ChainSimBuilder(std::string_view simulationName);

    ChainSimBuilder &simulation_length(std::size_t simulationLength);

    ChainSimBuilder &lead_time(std::size_t leadTime);

    ChainSimBuilder &average_demand(double averageDemand);

    ChainSimBuilder &starting_inventory(std::size_t startingInventory);

    ChainSim build();

private:
    ChainSim m_root{};

};


#endif //CHAINSIM_CHAINSIMBUILDER_H
