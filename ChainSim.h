
#ifndef CHAINSIM_CHAINSIM_H
#define CHAINSIM_CHAINSIM_H

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <algorithm>

#include "purchase_methods/PurchaseMethod.h"
#include "utils/ChainLogger.hpp"

using namespace std;

class ChainSim {
    friend class ChainSimBuilder;

    unsigned m_simulation_length{};
    unsigned m_starting_inventory{};
    unsigned m_lead_time{};
    double m_current_demand{};

    std::string m_simulation_name;
    vector<string> m_records_columns;
    unordered_map<string, vector<long>> m_records;

    unsigned m_logging_level{0};
    ChainLogger m_logger{};

    ChainSim();

public:

    void initialize_simulation();

    void simulate(const PurchaseMethod &purchaseMethod);

    [[nodiscard]] unordered_map<string, vector<long>> get_simulation_records() const;
};


#endif //CHAINSIM_CHAINSIM_H
