#include <gtest/gtest.h>
#include "../ChainSimBuilder.h"

TEST(ChainSimBuilderTest, BasicBuilder)
{
        auto sim = qz::ChainSimBuilder("TestSim")
                       .simulation_length(30)
                       .lead_time(5)
                       .average_demand(50.0)
                       .starting_inventory(100)
                       .logging_level(0)
                       .build();

        auto records = sim.get_simulation_records();

        // Verify the simulation was configured correctly
        EXPECT_EQ(records["inventory_quantity"].size(), 30);
        EXPECT_EQ(records["demand_quantity"].size(), 30);
        EXPECT_EQ(records["procurement_quantity"].size(), 30);
        EXPECT_EQ(records["purchase_quantity"].size(), 30);
        EXPECT_EQ(records["sale_quantity"].size(), 30);
        EXPECT_EQ(records["lost_sale_quantity"].size(), 30);
}

TEST(ChainSimBuilderTest, BuilderChaining)
{
        // Test that builder methods can be chained in any order
        auto sim1 = qz::ChainSimBuilder("TestSim")
                        .simulation_length(30)
                        .lead_time(5)
                        .build();

        auto sim2 = qz::ChainSimBuilder("TestSim")
                        .lead_time(5)
                        .simulation_length(30)
                        .build();

        // Both simulations should be equivalent
        EXPECT_EQ(sim1.get_simulation_records()["inventory_quantity"].size(),
                  sim2.get_simulation_records()["inventory_quantity"].size());
}

TEST(ChainSimBuilderTest, DefaultValues)
{
        // Test that unspecified values get reasonable defaults
        auto sim = qz::ChainSimBuilder("TestSim").build();

        auto records = sim.get_simulation_records();

        // Verify default values
        EXPECT_EQ(records["inventory_quantity"].size(), 30); // Default simulation length
        EXPECT_EQ(sim.get_current_day(), 1);                 // Starting day

        // Initialize and verify other defaults
        sim.initialize_simulation();
        records = sim.get_simulation_records();
        EXPECT_EQ(records["demand_quantity"][0], 50);   // Default demand
        EXPECT_EQ(records["inventory_quantity"][0], 0); // Default starting inventory
}

TEST(ChainSimBuilderTest, ErrorHandling)
{
        // Test empty simulation name
        EXPECT_THROW({ qz::ChainSimBuilder(""); }, std::invalid_argument);

        // Test invalid simulation length
        EXPECT_THROW({ qz::ChainSimBuilder("Test").simulation_length(0); }, std::invalid_argument);

        // Test invalid lead time
        EXPECT_THROW({ qz::ChainSimBuilder("Test").lead_time(0); }, std::invalid_argument);

        // Test invalid demand
        EXPECT_THROW({ qz::ChainSimBuilder("Test").average_demand(-1.0); }, std::invalid_argument);

        // Test invalid logging level
        EXPECT_THROW({ qz::ChainSimBuilder("Test").logging_level(3); }, std::invalid_argument);
}