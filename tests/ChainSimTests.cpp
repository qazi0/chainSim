#include <gtest/gtest.h>
#include "../ChainSim.h"
#include "../ChainSimBuilder.h"
#include "../purchase_policies/PurchaseROP.h"

class ChainSimTest : public ::testing::Test
{
protected:
    qz::ChainSim createDefaultSimulation()
    {
        return qz::ChainSimBuilder("TestSim")
            .simulation_length(30)
            .lead_time(5)
            .average_demand(50.0)
            .starting_inventory(100)
            .logging_level(0)
            .build();
    }

    qz::ChainSim createCustomSimulation(uint64_t starting_inventory,
                                        double demand)
    {
        return qz::ChainSimBuilder("TestSim")
            .simulation_length(30)
            .lead_time(5)
            .average_demand(demand)
            .starting_inventory(starting_inventory)
            .logging_level(0)
            .build();
    }
};

TEST_F(ChainSimTest, InitializationTest)
{
    auto sim = createDefaultSimulation();
    sim.initialize_simulation();

    auto records = sim.get_simulation_records();

    // Check initial inventory
    EXPECT_EQ(records["inventory_quantity"][0], 100);

    // Check demand initialization
    EXPECT_EQ(records["demand_quantity"][0], 50);

    // Check other metrics are zero initially
    EXPECT_EQ(records["procurement_quantity"][0], 0);
    EXPECT_EQ(records["purchase_quantity"][0], 0);
    EXPECT_EQ(records["sale_quantity"][0], 0);
    EXPECT_EQ(records["lost_sale_quantity"][0], 0);
}

TEST_F(ChainSimTest, BasicSimulationFlow)
{
    auto sim = createDefaultSimulation();
    sim.initialize_simulation();

    PurchaseROP policy(5, 50.0);
    sim.simulate(policy);

    auto records = sim.get_simulation_records();

    // Verify simulation ran for the full length
    EXPECT_EQ(records["inventory_quantity"].size(), 30);

    // Basic sanity checks
    for (size_t i = 1; i < records["inventory_quantity"].size(); ++i)
    {
        // Inventory should never be negative
        EXPECT_GE(records["inventory_quantity"][i], 0);

        // Sales + lost sales should equal demand
        EXPECT_EQ(records["sale_quantity"][i] + records["lost_sale_quantity"][i],
                  records["demand_quantity"][i]);
    }
}

TEST_F(ChainSimTest, EdgeCaseInventoryManagement)
{
    // Test zero inventory scenario
    auto zero_inventory_sim = createCustomSimulation(0, 100.0);
    zero_inventory_sim.initialize_simulation();

    PurchaseROP policy(5, 50.0);
    zero_inventory_sim.simulate_days(policy, 1);

    auto records = zero_inventory_sim.get_simulation_records();

    // All demand should become lost sales when inventory is zero
    EXPECT_EQ(records["lost_sale_quantity"][1], records["demand_quantity"][1]);
    EXPECT_EQ(records["sale_quantity"][1], 0);

    // Test very high inventory scenario
    auto high_inventory_sim = createCustomSimulation(
        std::numeric_limits<long>::max() - 1000, 50.0);
    high_inventory_sim.initialize_simulation();
    high_inventory_sim.simulate_days(policy, 1);

    auto high_records = high_inventory_sim.get_simulation_records();

    // Ensure no overflow occurs and sales are processed correctly
    EXPECT_GT(high_records["inventory_quantity"][1], 0);
    EXPECT_EQ(high_records["sale_quantity"][1], 50);
}

TEST_F(ChainSimTest, DemandPatterns)
{
    // Create simulation with varying demand pattern
    std::vector<double> seasonal_demand = {
        100.0, 120.0, 150.0, 200.0, // Peak season
        50.0, 30.0, 20.0, 10.0,     // Low season
        100.0, 120.0, 150.0, 200.0  // Peak season again
    };

    for (size_t i = 0; i < seasonal_demand.size(); ++i)
    {
        auto sim = createCustomSimulation(1000, seasonal_demand[i]); // High initial inventory
        sim.initialize_simulation();

        PurchaseROP policy(5, seasonal_demand[i]);
        sim.simulate_days(policy, 1);

        auto records = sim.get_simulation_records();

        // Verify demand handling
        EXPECT_EQ(records["sale_quantity"][1] + records["lost_sale_quantity"][1],
                  static_cast<long>(seasonal_demand[i]));
    }
}

TEST_F(ChainSimTest, LeadTimeVariations)
{
    // Test short lead time
    {
        PurchaseROP policy(1, 50.0);
        auto short_lead_sim = qz::ChainSimBuilder("ShortLeadTime")
                                  .simulation_length(30)
                                  .lead_time(1) // Very short lead time
                                  .average_demand(50.0)
                                  .starting_inventory(0) // Start with zero to trigger immediate order
                                  .logging_level(0)
                                  .build();

        short_lead_sim.initialize_simulation();
        short_lead_sim.simulate_days(policy, 2); // Simulate 2 days

        auto records = short_lead_sim.get_simulation_records();
        // Verify quick replenishment
        EXPECT_GT(records["procurement_quantity"][2], 0);
    }

    // Test long lead time
    {
        PurchaseROP policy(29, 50.0);
        auto long_lead_sim = qz::ChainSimBuilder("LongLeadTime")
                                 .simulation_length(30)
                                 .lead_time(29) // Very long lead time
                                 .average_demand(50.0)
                                 .starting_inventory(0) // Start with zero to trigger immediate order
                                 .build();

        long_lead_sim.initialize_simulation();
        long_lead_sim.simulate(policy);

        auto records = long_lead_sim.get_simulation_records();
        // Verify delayed replenishment
        EXPECT_EQ(records["procurement_quantity"][29], 0);
    }
}

TEST_F(ChainSimTest, ErrorConditions)
{
    // Test invalid simulation length
    EXPECT_THROW(
        qz::ChainSimBuilder("InvalidLength")
            .simulation_length(0)
            .build(),
        std::invalid_argument);

    // Test negative demand
    EXPECT_THROW(
        qz::ChainSimBuilder("NegativeDemand")
            .average_demand(-50.0)
            .build(),
        std::invalid_argument);

    // Test invalid lead time
    EXPECT_THROW(
        qz::ChainSimBuilder("InvalidLeadTime")
            .lead_time(std::numeric_limits<uint64_t>::max())
            .build(),
        std::invalid_argument);
}