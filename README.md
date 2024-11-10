# ChainSim - Supply Chain Inventory Simulation

ChainSim is a C++ library for simulating and analyzing supply chain inventory management systems. It provides a flexible framework for testing different purchasing policies and analyzing their effectiveness under various conditions.

## Overview

ChainSim simulates day-by-day inventory management, incorporating:
- Demand fulfillment
- Purchase ordering
- Lead time delays
- Stock tracking
- Lost sales recording

## Supported Purchase Policies

### 1. ROP (Re-Order Point) with Constant Rate
The ROP policy triggers a fixed-size order when inventory position falls below a calculated reorder point.

```cpp
PurchaseROP policy(
    5,     // lead time
    50.0   // average daily demand
);
```

- **Reorder Point Calculation**: `ROP = (Lead Time × Average Daily Demand) + Safety Stock`
- **Safety Stock**: `Lead Time × Average Daily Demand`
- **Order Quantity**: `Lead Time × Average Daily Demand`

### 2. EOQ (Economic Order Quantity)
The EOQ policy balances ordering and holding costs to determine optimal order quantities.

```cpp
PurchaseEOQ policy(
    5,      // lead time
    50.0,   // average daily demand
    100.0,  // ordering cost
    0.2     // holding cost rate
);
```

- **EOQ Formula**: `sqrt((2 × Annual Demand × Ordering Cost) / Holding Cost)`
- **Reorder Point**: `(Lead Time × Average Daily Demand) + Safety Stock`
- **Safety Stock**: `50% of Lead Time Demand`

### 3. TPOP (Time-Phased Order Point)
The TPOP policy reviews inventory at fixed intervals and orders up to a target level.

```cpp
PurchaseTPOP policy(
    5,     // lead time
    50.0,  // average daily demand
    7      // review period (days)
);
```

- **Target Level**: `Expected Demand during Protection Interval + Safety Stock`
- **Protection Interval**: `Review Period + Lead Time`
- **Safety Stock**: `Average Daily Demand × sqrt(Protection Interval)`

## Usage Example

```cpp
#include "ChainSimBuilder.h"
#include "purchase_policies/PurchaseROP.h"

int main() {
    // Configure simulation
    auto sim = qz::ChainSimBuilder("MySimulation")
        .simulation_length(30)
        .lead_time(5)
        .average_demand(50.0)
        .starting_inventory(100)
        .logging_level(1)
        .build();

    sim.initialize_simulation();
    
    // Create and run policy
    PurchaseROP policy(5, 50.0);
    sim.simulate(policy);
    
    // Get results
    auto results = sim.get_simulation_records();
}
```

## Building and Running

### Prerequisites
- CMake 3.15 or higher
- C++17 compliant compiler
- Git (for fetching dependencies)

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/yourusername/chainsim.git
cd chainsim
```

2. Create build directory:
```bash
mkdir build && cd build
```

3. Configure and build:
```bash
cmake ..
make
```

4. Run tests:
```bash
ctest
```

### Command Line Options

The simulator supports various configuration parameters:

```bash
./chainsim [options]
```

Options:
- `--log_level <0-2>`: Logging verbosity
  - 0: Silent
  - 1: Basic info (default)
  - 2: Detailed debug
  
- `--simulation_length <days>`: Duration of simulation (default: 30)
- `--average_demand <units>`: Mean demand per time unit (default: 50.0)
- `--std_demand <units>`: Standard deviation of demand (default: 10.0)
- `--average_lead_time <days>`: Average delivery delay (default: 5)
- `--starting_inventory <units>`: Initial stock level (default: 0)
- `--purchase_period <days>`: Review period for periodic policies (default: 7)
- `--minimum_lot_size <units>`: Minimum order quantity (default: 10)
- `--seed <number>`: PRNG seed for demand generation (default: 7)
- `--output_file <path>`: CSV output file path (default: "simulation_records.csv")
- `--policy <name>`: Purchase policy selection
  - ROP: Reorder Point
  - TPOP: Time Phased Order Point
  - EOQ: Economic Order Quantity
  - EBQ: Economic Batch Quantity
  - CUSTOM: Custom policy
- `--deterministic`: Use fixed values instead of random sampling

Example:
```bash
./chainsim --policy ROP --simulation_length 60 --average_demand 100 --log_level 1
```

## Output Format

The simulation generates a CSV file containing:
- `inventory_quantity`: Current stock level
- `demand_quantity`: Customer demand
- `procurement_quantity`: Incoming deliveries
- `purchase_quantity`: New orders placed
- `sale_quantity`: Fulfilled demand
- `lost_sale_quantity`: Unfulfilled demand

Example output:
```csv
Day,inventory_quantity,demand_quantity,procurement_quantity,purchase_quantity,sale_quantity,lost_sale_quantity
0,100,50,0,0,50,0
1,50,50,0,250,50,0
2,0,50,0,0,0,50
...
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

[License information should be added here]

## Authors

[Author information should be added here]