
#ifndef CHAINSIM_PURCHASEPOLICY_H
#define CHAINSIM_PURCHASEPOLICY_H

#include <string>
#include <unordered_map>
#include <vector>

class PurchasePolicy {
public:
    PurchasePolicy() = default;

    [[nodiscard]] virtual long
    get_purchase(const std::unordered_map<std::string, std::vector<long>> &pastRecords, unsigned current_day) const = 0;

    virtual std::string name() const = 0;
};


#endif //CHAINSIM_PURCHASEPOLICY_H
