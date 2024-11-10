#ifndef CHAINSIM_PURCHASETPOP_H
#define CHAINSIM_PURCHASETPOP_H

#include "PurchasePolicy.h"
#include <cmath>
#include <stdexcept>

class PurchaseTPOP : public PurchasePolicy
{
public:
    PurchaseTPOP(unsigned leadTime, double avgDemand, unsigned reviewPeriod);

    [[nodiscard]] long get_purchase(const simulation_records_t &pastRecords,
                                    unsigned current_day) const override;

    [[nodiscard]] std::string name() const override;

private:
    unsigned m_lead_time;
    double m_average_daily_demand;
    unsigned m_review_period;
    double m_target_level;

    void calculate_target_level();
    void validate_parameters() const;
    [[nodiscard]] bool is_review_day(unsigned day) const;
    [[nodiscard]] std::string get_calculation_details(
        const simulation_records_t &pastRecords,
        unsigned current_day) const override;
};

#endif // CHAINSIM_PURCHASETPOP_H
