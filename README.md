# ChainSim

A C++ library for simulating supply chain inventory management systems.

## Overview

ChainSim simulates daily inventory operations including demand fulfillment, purchase ordering, lead time delays, and stock tracking.

## Purchase Policies

### ROP (Re-Order Point)
Places fixed-size orders when inventory falls below a calculated reorder point.

```cpp
PurchaseROP policy(5, 50.0);  // lead time, average daily demand
```

### EOQ (Economic Order Quantity)
Calculates optimal order quantities based on ordering and holding costs.

```cpp
PurchaseEOQ policy(5, 50.0, 100.0, 0.2);  // lead time, demand, ordering cost, holding cost
```

### TPOP (Time-Phased Order Point)
Reviews inventory at fixed intervals and orders up to a target level.

```cpp
PurchaseTPOP policy(5, 50.0, 7);  // lead time, average demand, review period
```

## Usage

```cpp
#include "ChainSimBuilder.h"
#include "purchase_policies/PurchaseROP.h"

int main() {
    auto sim = qz::ChainSimBuilder("MySimulation")
        .simulation_length(30)
        .lead_time(5)
        .average_demand(50.0)
        .starting_inventory(100)
        .build();

    sim.initialize_simulation();

    PurchaseROP policy(5, 50.0);
    sim.simulate(policy);

    auto results = sim.get_simulation_records();
}
```

## Build

### Requirements
- CMake 3.15+
- C++17 compiler
- Git

### Commands

```bash
mkdir build && cd build
cmake ..
make
ctest  # Run tests
```

## Command Line Options

```bash
./chainsim [options]
```

Key options:
- `--policy <ROP|TPOP|EOQ|EBQ|CUSTOM>`: Purchase policy
- `--simulation_length <days>`: Simulation duration
- `--average_demand <units>`: Mean demand per time unit
- `--lead_time <days>`: Average delivery delay
- `--starting_inventory <units>`: Initial stock level
- `--log_level <0-2>`: Logging verbosity

Example:
```bash
./chainsim --policy ROP --simulation_length 60 --average_demand 100
```

## Output

The simulation generates a CSV file with daily records:
- inventory_quantity
- demand_quantity
- procurement_quantity
- purchase_quantity
- sale_quantity
- lost_sale_quantity