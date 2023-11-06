
#ifndef CHAINSIM_PURCHASEPOLICY_H
#define CHAINSIM_PURCHASEPOLICY_H

#include <string>
#include <unordered_map>
#include <vector>

class PurchasePolicy {
public:
    using simulation_records_t = std::unordered_map<std::string, std::vector<long>>;

    PurchasePolicy() = default;

    [[nodiscard]] virtual long
    get_purchase(const simulation_records_t &pastRecords, unsigned current_day) const = 0;

    [[nodiscard]] virtual std::string name() const = 0;
};


#endif //CHAINSIM_PURCHASEPOLICY_H
