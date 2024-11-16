# ChainSim Supply Chain Simulator

A powerful supply chain simulation tool with a modern web interface, combining Qt backend services with a Next.js frontend for comprehensive supply chain analysis and visualization.

## 📸 Screenshots

### Configuration Interface
![Configuration Panel](/res/config.png)
Configure your simulation with an intuitive interface. Set inventory policies, demand distributions, and key parameters like lead time, starting inventory, and simulation duration.

### Real-Time Visualization
![Simulation Results](/res/plot.png)
Track key metrics through interactive plots. Monitor inventory levels, demand patterns, procurement decisions, and lost sales in real-time. The combined view provides a comprehensive overview of your supply chain's performance.

### Performance Analysis
![Analysis Report](/res/report.png)
Get detailed performance metrics including service level, average inventory, total lost sales, and order analysis. Mathematical formulas provide transparency into calculations.

## 🚀 Quick Start

### Local Development

1. **Backend Setup**
```bash
# Build the Qt backend
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run the backend server
./ChainSimQServe --server --log_level 2
```

2. **Frontend Setup**
```bash
# Navigate to frontend directory
cd chainsim-ui

# Install dependencies
npm install

# Start development server
npm run dev
```

3. **Access the Application**
- Frontend: http://localhost:3000
- Backend API: http://localhost:47761

### Docker Deployment

```bash
# Build the container
docker build -t chainsim .

# Run locally
docker run -p 3000:3000 -p 47761:47761 chainsim

# Deploy to Google Cloud Run
gcloud run deploy chainsim \
  --image gcr.io/[PROJECT_ID]/chainsim \
  --platform managed \
  --allow-unauthenticated \
  --port 3000
```

## 🏗 Architecture

### Backend (Qt C++)
- Qt 6.8.0 HTTP server
- RESTful API endpoints
- High-performance simulation engine
- Real-time data processing

### Frontend (Next.js)
- Next.js 14 with TypeScript
- Material-UI components
- Real-time data visualization
- Responsive design

## 🎯 Features

### Simulation Configuration

#### Supply Chain Parameters
- Simulation duration
- Lead time configuration
- Inventory management
- Lot size optimization
- Stochastic/Deterministic modes

#### Demand Distribution Models
- Fixed (Constant)
- Normal
- Gamma
- Poisson
- Uniform

### Inventory Management Policies

1. **ROP (Reorder Point)**
   - Dynamic reorder triggers
   - Safety stock calculation

2. **EOQ (Economic Order Quantity)**
   - Cost optimization
   - Order quantity calculation

3. **TPOP (Time-Phased Order Point)**
   - Time-based ordering
   - Period optimization

### Analytics & Visualization

#### Interactive Charts
- Inventory levels
- Demand patterns
- Order quantities
- Stockout analysis
- Multi-metric views

#### Performance Metrics
- Service level analysis
- Inventory turnover
- Cost analysis
- Order statistics
- Demand patterns

## 📊 Theoretical Framework

### Inventory Management Policies

#### 1. ROP (Reorder Point) System
The ROP system triggers an order when inventory position falls below a specified point. It considers lead time demand and safety stock.

**Mathematical Model:**
```latex
ROP = μ_L + zσ_L

where:
μ_L = D × L (Average demand during lead time)
σ_L = σ_d × √L (Standard deviation during lead time)
D = Average daily demand
L = Lead time
z = Safety factor (based on service level)
σ_d = Daily demand standard deviation
```

**Safety Stock Calculation:**
```latex
Safety Stock = z × σ_L
```

#### 2. EOQ (Economic Order Quantity)
EOQ balances ordering and holding costs to determine optimal order quantity.

**Mathematical Model:**
```latex
EOQ = √((2DS)/H)

where:
D = Annual demand
S = Fixed ordering cost
H = Annual holding cost per unit
```

**Total Annual Cost:**
```latex
TC = (D/Q)S + (Q/2)H + CD

where:
Q = Order quantity
C = Unit cost
```

#### 3. TPOP (Time-Phased Order Point)
TPOP uses periodic review with dynamic order quantities based on projected inventory position.

**Mathematical Model:**
```latex
Order Quantity = Max(0, IP_target - IP_current)

where:
IP_target = μ_L+R + z × σ_L+R
IP_current = OH + OO - BO
R = Review period
OH = On-hand inventory
OO = On-order quantity
BO = Backorders
```

### Demand Distribution Models

#### 1. Normal Distribution
Used for stable demand patterns with symmetric variation.

```latex
f(x) = (1/(σ√(2π))) × e^(-(x-μ)²/(2σ²))

where:
μ = Mean demand
σ = Standard deviation
```

**Properties:**
- Symmetric around mean
- 68-95-99.7 rule applies
- Range: (-∞, +∞)

#### 2. Gamma Distribution
Suitable for right-skewed demand patterns and lead times.

```latex
f(x) = (x^(k-1) × e^(-x/θ))/(θ^k × Γ(k))

where:
k = Shape parameter
θ = Scale parameter
Γ = Gamma function
```

**Properties:**
- Always positive
- Right-skewed
- Range: [0, +∞)

#### 3. Poisson Distribution
Models discrete events in fixed time intervals.

```latex
P(X = k) = (λ^k × e^(-λ))/k!

where:
λ = Average rate of events
k = Number of events
```

**Properties:**
- Discrete distribution
- Mean equals variance
- Range: {0, 1, 2, ...}

#### 4. Uniform Distribution
Used for demand with equal probability within bounds.

```latex
f(x) = 1/(b-a) for a ≤ x ≤ b

where:
a = Lower bound
b = Upper bound
```

**Properties:**
- Constant probability density
- Range: [a, b]

### Demand Sampling Implementation

#### Random Number Generation
The simulator uses the Mersenne Twister algorithm for high-quality random number generation:

```cpp
std::mt19937 generator(seed);
```

#### Distribution Transformations

1. **Normal Distribution:**
```cpp
std::normal_distribution<double>(mean, stddev)
```

2. **Gamma Distribution:**
```cpp
std::gamma_distribution<double>(shape, scale)
```

3. **Poisson Distribution:**
```cpp
std::poisson_distribution<double>(lambda)
```

4. **Uniform Distribution:**
```cpp
std::uniform_real_distribution<double>(min, max)
```

### Performance Metrics

#### 1. Service Level
```latex
Service Level = (Total Demand - Lost Sales)/Total Demand × 100%
```

#### 2. Inventory Turnover
```latex
Turnover Ratio = Annual Sales/Average Inventory
```

#### 3. Average Inventory
```latex
Average Inventory = Σ(Daily Inventory)/Number of Days
```

#### 4. Total Cost
```latex
Total Cost = Ordering Costs + Holding Costs + Stockout Costs

where:
Ordering Costs = Number of Orders × Cost per Order
Holding Costs = Average Inventory × Holding Cost Rate
Stockout Costs = Lost Sales × Stockout Cost per Unit
```

## 🛠 Technical Stack

### Backend Technologies
- Qt 6.8.0
- C++17
- CMake build system
- Qt HTTP Server
- WebSocket support

### Frontend Technologies
- Next.js 14
- TypeScript
- Material-UI v5
- Recharts
- Framer Motion

### DevOps & Deployment
- Docker containerization
- Google Cloud Run
- GitHub Actions CI/CD
- Multi-stage builds

## 📦 Installation Requirements

### System Requirements
- Docker 20.10+
- Node.js 18+
- Qt 6.8.0
- CMake 3.16+
- Python 3.10+

### Environment Variables
```env
CORS_DOMAIN=your-domain.com
API_URL=http://localhost:47761
ALLOWED_ORIGINS=http://localhost:3000,http://localhost:47761
```

## 🔧 Configuration

### Backend Configuration
```bash
# Server options
--server          # Run in server mode
--log_level 2     # Detailed logging
--port 47761      # Custom port
```

### Frontend Configuration
```typescript
// next.config.js options
{
  output: 'standalone',
  async rewrites() {
    return [
      {
        source: '/api/:path*',
        destination: '${process.env.NEXT_PUBLIC_API_URL}/:path*'
      }
    ]
  }
}
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Implement changes
4. Add tests
5. Submit pull request

## 🆘 Support

- GitHub Issues: [Report a bug](https://github.com/qazi0/chainsim/issues)
- Community: [Discussions](https://github.com/qazi0/chainsim/discussions)

## 🙏 Acknowledgments

- Qt Framework team
- Next.js community
- Material-UI contributors
- All open-source contributors

---
