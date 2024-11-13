
#ifndef CHAINSIM_CHAINSIMBUILDER_H
#define CHAINSIM_CHAINSIMBUILDER_H

#include "ChainSim.h"

namespace qz {
class ChainSimBuilder {
public:
    explicit ChainSimBuilder(std::string_view simulationName);

    ChainSimBuilder &simulation_length(uint64_t simulationLength);

    ChainSimBuilder &lead_time(uint64_t leadTime);

    ChainSimBuilder &average_demand(double averageDemand);

    ChainSimBuilder &starting_inventory(uint64_t startingInventory);

    ChainSimBuilder &logging_level(unsigned loggingLevel);

    qz::ChainSim build();

private:
    ChainSim m_root;

};
}

#endif //CHAINSIM_CHAINSIMBUILDER_H
