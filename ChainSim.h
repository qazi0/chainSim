
#ifndef CHAINSIM_CHAINSIM_H
#define CHAINSIM_CHAINSIM_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "purchase_policies/PurchasePolicy.h"
#include "utils/ChainLogger.hpp"


namespace qz {
    class ChainSim {
    public:
        using simulation_records_t = std::unordered_map<std::string, std::vector<long>>;

        void initialize_simulation();

        void simulate(const PurchasePolicy &purchasePolicy);

        [[nodiscard]] simulation_records_t get_simulation_records() const;

    private:
        ChainSim();

        friend class ChainSimBuilder;

        uint64_t m_simulation_length{};
        uint64_t m_starting_inventory{};
        uint64_t m_lead_time{};
        double m_current_demand{};

        std::string m_simulation_name;
        std::vector<std::string> m_records_columns;
        simulation_records_t m_records;

        uint64_t m_logging_level{0};
        ChainLogger m_logger{};
    };
}

#endif //CHAINSIM_CHAINSIM_H
