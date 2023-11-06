//
// Created by blakberrisigma on 03/11/2023.
//

#ifndef CHAINSIM_PURCHASEMETHOD_H
#define CHAINSIM_PURCHASEMETHOD_H

#include <string>
#include <unordered_map>
#include <vector>

class PurchaseMethod {
public:
    PurchaseMethod() = default;

    [[nodiscard]] virtual long
    get_purchase(const std::unordered_map<std::string, std::vector<long>> &pastRecords, unsigned current_day) const = 0;

    virtual std::string name() const = 0;
};


#endif //CHAINSIM_PURCHASEMETHOD_H
