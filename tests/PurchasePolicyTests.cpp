#include <gtest/gtest.h>
#include "../purchase_policies/PurchaseROP.h"
#include "../purchase_policies/PurchaseEOQ.h"
#include "../purchase_policies/PurchaseTPOP.h"
#include <unordered_map>
#include <vector>
#include <cmath>

class PurchasePolicyTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        records["inventory_quantity"] = std::vector<long>(30, 0);
        records["demand_quantity"] = std::vector<long>(30, 50);
        records["procurement_quantity"] = std::vector<long>(30, 0);
        records["purchase_quantity"] = std::vector<long>(30, 0);
        records["sale_quantity"] = std::vector<long>(30, 0);
        records["lost_sale_quantity"] = std::vector<long>(30, 0);
    }

    std::unordered_map<std::string, std::vector<long>> records;
};

TEST_F(PurchasePolicyTest, ROPBasicFunctionality)
{
    PurchaseROP policy(5, 50.0);

    // Test when inventory is below reorder point
    records["inventory_quantity"][10] = 100;
    auto purchase = policy.get_purchase(records, 10);
    EXPECT_GT(purchase, 0);

    // Test when inventory is above reorder point
    records["inventory_quantity"][11] = 1000;
    purchase = policy.get_purchase(records, 11);
    EXPECT_EQ(purchase, 0);
}

TEST_F(PurchasePolicyTest, ROPReorderPoint)
{
    PurchaseROP policy(5, 50.0);

    // Test various inventory levels around the reorder point
    std::vector<long> testLevels = {0, 100, 250, 500};

    for (auto level : testLevels)
    {
        records["inventory_quantity"][0] = level;
        auto purchase = policy.get_purchase(records, 0);

        if (level <= 500)
        { // Assuming reorder point is around 250
            EXPECT_GT(purchase, 0);
        }
        else
        {
            EXPECT_EQ(purchase, 0);
        }
    }
}

TEST_F(PurchasePolicyTest, EOQBasicFunctionality)
{
    PurchaseEOQ policy(5, 50.0, 100.0, 0.2);

    // Test initial order when inventory is low
    records["inventory_quantity"][0] = 100;
    auto purchase = policy.get_purchase(records, 0);

    // EOQ formula: sqrt((2 * annual demand * ordering cost) / holding cost)
    double expected_eoq = std::sqrt((2 * 50.0 * 365 * 100.0) / 0.2);
    EXPECT_NEAR(purchase, expected_eoq, expected_eoq * 0.1); // 10% tolerance
}

TEST_F(PurchasePolicyTest, EOQWithPipelineInventory)
{
    PurchaseEOQ policy(5, 50.0, 100.0, 0.2);

    // Set up pipeline inventory
    records["inventory_quantity"][10] = 200;
    records["procurement_quantity"][11] = 100;
    records["procurement_quantity"][12] = 100;

    auto purchase = policy.get_purchase(records, 10);

    // With sufficient pipeline inventory, should not order
    EXPECT_EQ(purchase, 0);
}

TEST_F(PurchasePolicyTest, EOQParameterValidation)
{
    // Test invalid parameters
    EXPECT_THROW(PurchaseEOQ(0, 50.0, 100.0, 0.2), std::invalid_argument);
    EXPECT_THROW(PurchaseEOQ(5, -50.0, 100.0, 0.2), std::invalid_argument);
    EXPECT_THROW(PurchaseEOQ(5, 50.0, -100.0, 0.2), std::invalid_argument);
    EXPECT_THROW(PurchaseEOQ(5, 50.0, 100.0, -0.2), std::invalid_argument);
}

TEST_F(PurchasePolicyTest, TPOPBasicFunctionality)
{
    PurchaseTPOP policy(5, 50.0, 7);

    // Test ordering pattern
    for (unsigned day = 0; day < 30; ++day)
    {
        records["inventory_quantity"][day] = 200; // Set consistent inventory level
        auto purchase = policy.get_purchase(records, day);

        if (day % 7 == 0)
        { // Review day
            EXPECT_GT(purchase, 0);
        }
        else
        {
            EXPECT_EQ(purchase, 0);
        }
    }
}

TEST_F(PurchasePolicyTest, TPOPTargetLevel)
{
    PurchaseTPOP policy(5, 50.0, 7);

    // Test target level calculation
    records["inventory_quantity"][7] = 100; // Low inventory on review day
    auto purchase = policy.get_purchase(records, 7);

    // Target level should cover review period + lead time + safety stock
    double expected_min = 50.0 * (7 + 5); // Minimum coverage needed
    EXPECT_GT(purchase, expected_min);
}

TEST_F(PurchasePolicyTest, TPOPWithPipelineInventory)
{
    PurchaseTPOP policy(5, 50.0, 7);

    // Set up pipeline inventory
    records["inventory_quantity"][7] = 200;
    records["procurement_quantity"][8] = 100;
    records["procurement_quantity"][9] = 100;

    auto purchase = policy.get_purchase(records, 7);

    // Order quantity should consider pipeline inventory
    double expected_without_pipeline = 50.0 * (7 + 5); // Basic coverage
    EXPECT_LT(purchase, expected_without_pipeline);
}

TEST_F(PurchasePolicyTest, TPOPParameterValidation)
{
    // Test invalid parameters
    EXPECT_THROW(PurchaseTPOP(0, 50.0, 7), std::invalid_argument);
    EXPECT_THROW(PurchaseTPOP(5, -50.0, 7), std::invalid_argument);
    EXPECT_THROW(PurchaseTPOP(5, 50.0, 0), std::invalid_argument);
}

TEST_F(PurchasePolicyTest, ROPWithVariableDemand)
{
    PurchaseROP policy(5, 50.0);

    // Test with increasing demand pattern
    for (unsigned day = 0; day < 30; ++day)
    {
        records["demand_quantity"][day] = 50 + day; // Linear increase
    }

    // Verify more frequent ordering with higher demand
    int order_count = 0;
    for (unsigned day = 0; day < 30; ++day)
    {
        if (policy.get_purchase(records, day) > 0)
        {
            order_count++;
        }
    }

    EXPECT_GT(order_count, 3); // Expect more orders due to higher demand
}

TEST_F(PurchasePolicyTest, ROPEdgeCases)
{
    PurchaseROP policy(5, 50.0);

    // Test with zero inventory
    records["inventory_quantity"][0] = 0;
    EXPECT_GT(policy.get_purchase(records, 0), 0);

    // Test with very high inventory
    records["inventory_quantity"][1] = 10000;
    EXPECT_EQ(policy.get_purchase(records, 1), 0);

    // Test with inventory exactly at reorder point
    records["inventory_quantity"][2] = 250; // Assuming reorder point is 250
    auto purchase = policy.get_purchase(records, 2);
    EXPECT_GT(purchase, 0);
}

TEST_F(PurchasePolicyTest, PolicyExceptions)
{
    // Test invalid lead time
    try
    {
        PurchaseROP policy(0, 50.0);
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument &e)
    {
        EXPECT_STREQ(e.what(), "Lead time must be greater than zero");
    }

    // Test invalid demand
    try
    {
        PurchaseROP policy(5, -50.0);
        FAIL() << "Expected std::invalid_argument";
    }
    catch (const std::invalid_argument &e)
    {
        EXPECT_STREQ(e.what(), "Average demand must be positive");
    }
}