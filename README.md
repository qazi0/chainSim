# ChainSim
A supply chain operations simulator, in C++.

### Build & Run
Generate Makefile with CMake as
```shell
mkdir build
cd build
cmake ..
```
Now compile with make/ninja:
```shell
make
# or 
# ninja
```
Run with
```shell
./ChainSim
```

### Simulation variables
- Inventory
- Demand
- Purchases
- Procurements
- Sales
- Lost Sales (Stock-outs)

### Supported Purchase Policies
- Re-Order Point (ROP)
- Time Phased Order Point (TPOP)
- Economic Order Quantity (EOQ)
- Economic Batch Quantity (EBQ)
- Custom

### Demand modes
- Deterministic (fixed.)
- Stochastic
  - Gaussian
  - Uniform
  - Gamma
- Custom (User input, CSV)
