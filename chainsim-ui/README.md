# ChainSim UI

A modern, responsive web interface for the ChainSim supply chain simulation tool built with Next.js, Material-UI, and TypeScript.

## Technical Stack

### Core Technologies
- **Next.js 14**: React framework for production
- **TypeScript**: Type-safe development
- **Material-UI (MUI)**: Component library and theming
- **Framer Motion**: Animation system
- **Recharts**: Data visualization
- **Canvas Confetti**: Special effects

### Key Features
- Responsive design with Material-UI
- Dark/Light theme support
- Real-time data visualization
- Client-side form validation
- Interactive charts with zoom capabilities
- Animated loading states and modals
- CSV data export functionality

## Features for End Users

### Simulation Configuration

#### Basic Parameters
- Simulation length (days)
- Average lead time
- Starting inventory
- Minimum lot size
- Random seed control
- Deterministic mode toggle

#### Demand Distribution Options
- Fixed (Constant) demand
- Normal distribution
  - Mean and standard deviation
- Gamma distribution
  - Shape and scale parameters
- Poisson distribution
  - Lambda parameter
- Uniform distribution
  - Min and max bounds

#### Inventory Policies
1. **ROP (Reorder Point)**
   - Basic reorder point system
   - Automatic order triggering

2. **EOQ (Economic Order Quantity)**
   - Ordering cost configuration
   - Holding cost rate
   - Optimal order quantity calculation

3. **TPOP (Time-Phased Order Point)**
   - Purchase period configuration
   - Time-based ordering system

### Visualization & Analysis

#### Interactive Charts
- **Inventory Levels**: Track stock over time
- **Demand & Sales**: Compare patterns
- **Procurement**: View order quantities
- **Lost Sales**: Analyze stockouts
- **Combined View**: All metrics together

#### Chart Features
- Zoom in/out capability
- Pan across time periods
- Interactive tooltips
- Dynamic scaling
- Reference areas
- Custom legend

#### Performance Metrics
- Service Level Percentage
- Average Inventory
- Total Lost Sales
- Total Purchases
- Average Daily Demand
- Inventory Turns
- Peak Inventory
- Minimum Inventory
- Number of Orders
- Average Order Size
- Average Order Interval

### Data Management

#### Export Options
- CSV download of all metrics
- Detailed tabular view
- Day-by-day breakdown of:
  - Inventory quantities
  - Demand levels
  - Procurement amounts
  - Purchase decisions
  - Sales figures
  - Lost sales data

#### Analysis Tools
- Statistical calculations
- Performance indicators
- Mathematical formulas display
- Trend analysis

### User Interface Features
- Responsive design for all devices
- Dark/Light mode toggle
- Smooth animations
- Loading indicators
- Error handling
- Form validation
- Scroll-to-top/bottom buttons
- Collapsible sections
- Interactive tooltips

## Development Setup

1. Install dependencies:
```bash
npm install
```

2. Required dependencies:
```json
{
  "@mui/material": "^5.x",
  "@mui/icons-material": "^5.x",
  "recharts": "^2.x",
  "framer-motion": "^10.x",
  "canvas-confetti": "^1.x",
  "@fontsource/ubuntu": "^5.x",
  "@fontsource/roboto": "^5.x",
  "react-katex": "^3.x"
}
```

3. Start development server:
```bash
npm run dev
```

## API Integration

The UI connects to a ChainSim backend server:
- Default port: 47761
- Endpoint: `/simulate`
- Method: POST
- CORS enabled
- JSON response format

## Error Handling

- Form validation errors
- Server connection issues
- Simulation failures
- Data validation
- User feedback

## Performance Considerations

- Optimized chart rendering
- Lazy loading of components
- Efficient state management
- Debounced inputs
- Memoized calculations

## Browser Support

- Modern browsers (Chrome, Firefox, Safari, Edge)
- Responsive design for mobile devices
- Progressive enhancement

## Contributing

1. Fork the repository
2. Create feature branch
3. Commit changes
4. Push to branch
5. Create Pull Request

## License

MIT License - see LICENSE file for details
