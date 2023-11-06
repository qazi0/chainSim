
#ifndef CHAINSIM_CHAINSIM_H
#define CHAINSIM_CHAINSIM_H

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <algorithm>

#include "purchase_policies/PurchasePolicy.h"
#include "utils/ChainLogger.hpp"

using namespace std;

class ChainSim {
public:
    using simulation_records_t = std::unordered_map<std::string, std::vector<long>>;

    void initialize_simulation();

    void simulate(const PurchasePolicy &purchasePolicy);

    [[nodiscard]] simulation_records_t get_simulation_records() const;

private:
    ChainSim();

    friend class ChainSimBuilder;

    unsigned m_simulation_length{};
    unsigned m_starting_inventory{};
    unsigned m_lead_time{};
    double m_current_demand{};

    std::string m_simulation_name;
    std::vector<string> m_records_columns;
    simulation_records_t m_records;

    unsigned m_logging_level{0};
    ChainLogger m_logger{};
};


#endif //CHAINSIM_CHAINSIM_H
