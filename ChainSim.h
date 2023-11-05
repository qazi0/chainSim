//
// Created by blakberrisigma on 03/11/2023.
//

#ifndef CHAINSIM_CHAINSIM_H
#define CHAINSIM_CHAINSIM_H


#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "PurchaseMethod.h"

using namespace std;
    class ChainSim {
        unsigned m_simulation_length;
        unsigned m_lead_time;
        unsigned m_current_demand;

        vector<string> m_records_columns;
        unordered_map<string, vector<long>> m_records;

    public:
        ChainSim(unsigned simulationLength, unsigned leadTime, unsigned demand, unsigned startingInventory);
        void initialize_simulation(unsigned startingInventory = 0);
        void simulate(const PurchaseMethod& purchaseMethod);
        unordered_map<string, vector<long>> get_simulation_records() const;
    };



#endif //CHAINSIM_CHAINSIM_H
