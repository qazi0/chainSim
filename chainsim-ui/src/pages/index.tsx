import { useState, useEffect, useRef } from 'react';
import Head from 'next/head';
import {
    Container,
    Typography,
    Card,
    CardContent,
    Grid,
    TextField,
    MenuItem,
    Button,
    Box,
    Paper,
    Table,
    TableBody,
    TableCell,
    TableContainer,
    TableHead,
    TableRow,
    FormControlLabel,
    Switch,
    CircularProgress,
    Alert,
    Tabs,
    Tab,
} from '@mui/material';
import DownloadIcon from '@mui/icons-material/Download';
import {
    LineChart,
    Line,
    XAxis,
    YAxis,
    CartesianGrid,
    Tooltip,
    Legend,
    ResponsiveContainer,
    ReferenceArea,
} from 'recharts';
import '@fontsource/ubuntu/700.css';
import '@fontsource/roboto/400.css';
import { createTheme, ThemeProvider, useTheme } from '@mui/material/styles';
import Brightness4Icon from '@mui/icons-material/Brightness4';
import Brightness7Icon from '@mui/icons-material/Brightness7';
import KeyboardArrowUpIcon from '@mui/icons-material/KeyboardArrowUp';
import KeyboardArrowDownIcon from '@mui/icons-material/KeyboardArrowDown';
import ArticleIcon from '@mui/icons-material/Article';
import { Fab, IconButton } from '@mui/material';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import { Collapse, Accordion, AccordionSummary, AccordionDetails } from '@mui/material';
import 'katex/dist/katex.min.css';
import { InlineMath, BlockMath } from 'react-katex';
import { alpha } from '@mui/material/styles';
import { motion, AnimatePresence } from 'framer-motion';
import LoadingButton from '@mui/lab/LoadingButton';
import { PolicySelector } from '@/components/PolicySelector';
import { ZoomableChart } from '@/components/ZoomableChart';
import { SimulationConfig, ValidationErrors, SimulationResult } from '@/types';
import confetti from 'canvas-confetti';

interface SimulationConfig {
    simulation_length: number;
    average_lead_time: number;
    average_demand: number;
    std_demand: number;
    policy: 'ROP' | 'TPOP' | 'EOQ';
    starting_inventory: number;
    log_level: number;
    seed: number;
    minimum_lot_size: number;
    ordering_cost?: number;
    holding_cost?: number;
    purchase_period?: number;
    deterministic: boolean;
    demand_distribution: 'fixed' | 'normal' | 'gamma' | 'poisson' | 'uniform';
    gamma_shape?: number;
    gamma_scale?: number;
    uniform_min?: number;
    uniform_max?: number;
}

interface SimulationResult {
    inventory_quantity: number[];
    demand_quantity: number[];
    procurement_quantity: number[];
    purchase_quantity: number[];
    sale_quantity: number[];
    lost_sale_quantity: number[];
}

interface TabPanelProps {
    children?: React.ReactNode;
    index: number;
    value: number;
}

function TabPanel(props: TabPanelProps) {
    const { children, value, index, ...other } = props;

    return (
        <div
            role="tabpanel"
            hidden={value !== index}
            id={`simple-tabpanel-${index}`}
            aria-labelledby={`simple-tab-${index}`}
            {...other}
        >
            {value === index && (
                <Box sx={{ p: 3 }}>
                    {children}
                </Box>
            )}
        </div>
    );
}

const customTheme = (mode: 'light' | 'dark') => createTheme({
    palette: {
        mode,
        primary: {
            main: mode === 'light' ? '#1976d2' : '#90caf9',
        },
        background: {
            default: mode === 'light' ? '#f8fafc' : '#121212',
            paper: mode === 'light' ? '#ffffff' : '#1e1e1e',
        },
    },
    components: {
        MuiCard: {
            styleOverrides: {
                root: {
                    borderRadius: '16px',
                    boxShadow: mode === 'light'
                        ? '0 4px 6px -1px rgb(0 0 0 / 0.1), 0 2px 4px -2px rgb(0 0 0 / 0.1)'
                        : '0 4px 6px -1px rgb(255 255 255 / 0.1), 0 2px 4px -2px rgb(255 255 255 / 0.1)',
                    transition: 'all 0.3s ease',
                    border: `2px solid ${mode === 'light' ? '#e2e8f0' : '#2d3748'}`,
                    '&.major-card': {
                        border: `3px solid ${mode === 'light' ? '#cbd5e1' : '#4a5568'}`,
                        boxShadow: mode === 'light'
                            ? '0 10px 15px -3px rgb(0 0 0 / 0.15), 0 4px 6px -4px rgb(0 0 0 / 0.15)'
                            : '0 10px 15px -3px rgb(255 255 255 / 0.15), 0 4px 6px -4px rgb(255 255 255 / 0.15)',
                    },
                    '&:hover': {
                        transform: 'translateY(-2px)',
                        boxShadow: mode === 'light'
                            ? '0 20px 25px -5px rgb(0 0 0 / 0.1), 0 8px 10px -6px rgb(0 0 0 / 0.1)'
                            : '0 20px 25px -5px rgb(255 255 255 / 0.1), 0 8px 10px -6px rgb(255 255 255 / 0.1)',
                    },
                },
            },
        },
    },
});

const SimulationLoadingOverlay = ({ isVisible }: { isVisible: boolean }) => (
    <AnimatePresence>
        {isVisible && (
            <motion.div
                initial={{ opacity: 0 }}
                animate={{ opacity: 1 }}
                exit={{ opacity: 0 }}
                style={{
                    position: 'fixed',
                    top: 0,
                    left: 0,
                    right: 0,
                    bottom: 0,
                    backgroundColor: 'rgba(0, 0, 0, 0.7)',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                    zIndex: 9999,
                }}
            >
                <motion.div
                    initial={{ scale: 0.5 }}
                    animate={{ scale: 1 }}
                    exit={{ scale: 0.5 }}
                    style={{
                        backgroundColor: 'white',
                        padding: '2rem',
                        borderRadius: '1rem',
                        textAlign: 'center',
                    }}
                >
                    <CircularProgress size={60} />
                    <Typography variant="h6" sx={{ mt: 2, color: 'text.primary' }}>
                        Simulating the entire period...
                    </Typography>
                    <Typography variant="body2" sx={{ mt: 1, color: 'text.secondary' }}>
                        Crunching numbers and analyzing patterns
                    </Typography>
                </motion.div>
            </motion.div>
        )}
    </AnimatePresence>
);

const ThankYouModal = ({ isVisible, onClose }: { isVisible: boolean; onClose: () => void }) => (
    <AnimatePresence>
        {isVisible && (
            <motion.div
                initial={{ opacity: 0 }}
                animate={{ opacity: 1 }}
                exit={{ opacity: 0 }}
                style={{
                    position: 'fixed',
                    top: 0,
                    left: 0,
                    right: 0,
                    bottom: 0,
                    backgroundColor: 'rgba(0, 0, 0, 0.5)',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                    zIndex: 9999,
                }}
            >
                <motion.div
                    initial={{ scale: 0.5, opacity: 0 }}
                    animate={{ scale: 1, opacity: 1 }}
                    exit={{ scale: 0.5, opacity: 0 }}
                    style={{
                        backgroundColor: 'white',
                        padding: '2rem',
                        borderRadius: '1rem',
                        textAlign: 'center',
                        boxShadow: '0 4px 6px rgba(0, 0, 0, 0.1)',
                        maxWidth: '90%',
                        width: '400px',
                    }}
                >
                    <Typography variant="h4" component="h2" sx={{ mb: 2, color: 'primary.main' }}>
                        Thank You! 🎉
                    </Typography>
                    <Typography variant="body1" sx={{ color: 'text.secondary' }}>
                        We're glad you're enjoying ChainSim!
                    </Typography>
                </motion.div>
            </motion.div>
        )}
    </AnimatePresence>
);

function ScrollButtons() {
    const [showScrollButtons, setShowScrollButtons] = useState(false);

    useEffect(() => {
        const handleScroll = () => {
            setShowScrollButtons(window.scrollY > 400);
        };
        window.addEventListener('scroll', handleScroll);
        return () => window.removeEventListener('scroll', handleScroll);
    }, []);

    const scrollTo = (position: 'top' | 'bottom') => {
        window.scrollTo({
            top: position === 'top' ? 0 : document.documentElement.scrollHeight,
            behavior: 'smooth',
        });
    };

    if (!showScrollButtons) return null;

    return (
        <Box
            sx={{
                position: 'fixed',
                right: 20,
                bottom: 20,
                display: 'flex',
                flexDirection: 'column',
                gap: 1,
                zIndex: 1000,
            }}
        >
            <Fab
                size="small"
                color="primary"
                onClick={() => scrollTo('top')}
                sx={{ opacity: 0.9 }}
            >
                <KeyboardArrowUpIcon />
            </Fab>
            <Fab
                size="small"
                color="primary"
                onClick={() => scrollTo('bottom')}
                sx={{ opacity: 0.9 }}
            >
                <KeyboardArrowDownIcon />
            </Fab>
        </Box>
    );
}

// Add this interface for analysis results
interface AnalysisReport {
    serviceLevelPct: number;
    averageInventory: number;
    totalLostSales: number;
    totalPurchases: number;
    averageDailyDemand: number;
    totalSales: number;
    inventoryTurns: number;
    peakInventory: number;
    minimumInventory: number;
    numberOfOrders: number;
    averageOrderSize: number;
    averageOrderInterval: number;
    totalOrderQuantity: number;
}

// Add this interface for statistics metadata
interface StatisticMetadata {
    label: string;
    value: number;
    equation: string;
    description: string;
}

// Add this function after calculateAnalysis
const getStatisticsMetadata = (analysis: AnalysisReport): StatisticMetadata[] => {
    return [
        {
            label: 'Service Level (%)',
            value: analysis.serviceLevelPct,
            equation: '\\frac{\\text{Total Sales}}{\\text{Total Sales} + \\text{Total Lost Sales}} \\times 100',
            description: 'Percentage of demand that was successfully fulfilled'
        },
        {
            label: 'Average Inventory',
            value: analysis.averageInventory,
            equation: '\\frac{\\sum_{t=1}^{n} \\text{Inventory}_t}{n}',
            description: 'Mean inventory level across all time periods'
        },
        {
            label: 'Total Lost Sales',
            value: analysis.totalLostSales,
            equation: '\\sum_{t=1}^{n} \\text{Lost Sales}_t',
            description: 'Sum of all unfulfilled demand'
        },
        {
            label: 'Total Purchases',
            value: analysis.totalPurchases,
            equation: '\\sum_{t=1}^{n} \\text{Purchase}_t',
            description: 'Total quantity ordered over the simulation period'
        },
        {
            label: 'Average Daily Demand',
            value: analysis.averageDailyDemand,
            equation: '\\frac{\\sum_{t=1}^{n} \\text{Demand}_t}{n}',
            description: 'Mean daily demand across the simulation'
        },
        {
            label: 'Inventory Turns',
            value: analysis.inventoryTurns,
            equation: '\\frac{\\text{Total Sales}}{\\text{Average Inventory}}',
            description: 'Rate at which inventory is sold and replaced'
        },
        {
            label: 'Number of Orders',
            value: analysis.numberOfOrders,
            equation: '\\text{Count}(\\text{Purchase}_t > 0)',
            description: 'Total number of purchase orders placed'
        },
        {
            label: 'Average Order Size',
            value: analysis.averageOrderSize,
            equation: '\\frac{\\text{Total Purchases}}{\\text{Number of Orders}}',
            description: 'Mean quantity per order'
        },
        {
            label: 'Average Order Interval',
            value: analysis.averageOrderInterval,
            equation: '\\frac{n}{\\text{Number of Orders}}',
            description: 'Average time between orders'
        }
    ];
};

// Add this component for the statistic card
function StatisticCard({ statistic }: { statistic: StatisticMetadata }) {
    const [expanded, setExpanded] = useState(false);

    return (
        <Card
            sx={{
                mb: 1,
                cursor: 'pointer',
                '&:hover': {
                    boxShadow: 6
                }
            }}
        >
            <Accordion
                expanded={expanded}
                onChange={() => setExpanded(!expanded)}
                sx={{ boxShadow: 'none' }}
            >
                <AccordionSummary
                    expandIcon={<ExpandMoreIcon />}
                    sx={{
                        display: 'flex',
                        alignItems: 'center',
                        p: 2
                    }}
                >
                    <Box sx={{ display: 'flex', justifyContent: 'space-between', width: '100%', alignItems: 'center' }}>
                        <Typography variant="subtitle1" sx={{ fontWeight: 'bold' }}>
                            {statistic.label}
                        </Typography>
                        <Typography variant="h6" color="primary">
                            {statistic.value.toFixed(2)}
                        </Typography>
                    </Box>
                </AccordionSummary>
                <AccordionDetails sx={{ bgcolor: 'background.default', p: 2 }}>
                    <Typography variant="body2" gutterBottom>
                        {statistic.description}
                    </Typography>
                    <Box sx={{ mt: 2, p: 2, bgcolor: 'background.paper', borderRadius: 1 }}>
                        <BlockMath math={statistic.equation} />
                    </Box>
                </AccordionDetails>
            </Accordion>
        </Card>
    );
}

// Add this component for distribution-specific parameters
function DemandDistributionParams({ config, handleInputChange, validationErrors }: {
    config: SimulationConfig;
    handleInputChange: (field: keyof SimulationConfig) => (event: React.ChangeEvent<HTMLInputElement>) => void;
    validationErrors: ValidationErrors;
}) {
    switch (config.demand_distribution) {
        case 'normal':
            return (
                <>
                    <Grid item xs={12} sm={6}>
                        <TextField
                            fullWidth
                            label="Average Demand"
                            type="number"
                            value={config.average_demand}
                            onChange={handleInputChange('average_demand')}
                            error={!!validationErrors.average_demand}
                            helperText={validationErrors.average_demand || "Mean of the normal distribution"}
                            required
                        />
                    </Grid>
                    <Grid item xs={12} sm={6}>
                        <TextField
                            fullWidth
                            label="Standard Deviation of Demand"
                            type="number"
                            value={config.std_demand}
                            onChange={handleInputChange('std_demand')}
                            error={!!validationErrors.std_demand}
                            helperText={validationErrors.std_demand || "Standard deviation of the normal distribution"}
                            required
                        />
                    </Grid>
                </>
            );
        case 'gamma':
            return (
                <>
                    <Grid item xs={12} sm={6}>
                        <TextField
                            fullWidth
                            label="Shape Parameter (k)"
                            type="number"
                            value={config.gamma_shape}
                            onChange={handleInputChange('gamma_shape')}
                            helperText="Shape parameter of the gamma distribution"
                        />
                    </Grid>
                    <Grid item xs={12} sm={6}>
                        <TextField
                            fullWidth
                            label="Scale Parameter (θ)"
                            type="number"
                            value={config.gamma_scale}
                            onChange={handleInputChange('gamma_scale')}
                            helperText="Scale parameter of the gamma distribution"
                        />
                    </Grid>
                </>
            );
        case 'poisson':
            return (
                <Grid item xs={12} sm={6}>
                    <TextField
                        fullWidth
                        label="Average Demand (λ)"
                        type="number"
                        value={config.average_demand}
                        onChange={handleInputChange('average_demand')}
                        helperText="Mean (and variance) of the Poisson distribution"
                    />
                </Grid>
            );
        case 'uniform':
            return (
                <>
                    <Grid item xs={12} sm={6}>
                        <TextField
                            fullWidth
                            label="Minimum Demand"
                            type="number"
                            value={config.uniform_min}
                            onChange={handleInputChange('uniform_min')}
                            helperText="Minimum value of uniform distribution"
                        />
                    </Grid>
                    <Grid item xs={12} sm={6}>
                        <TextField
                            fullWidth
                            label="Maximum Demand"
                            type="number"
                            value={config.uniform_max}
                            onChange={handleInputChange('uniform_max')}
                            helperText="Maximum value of uniform distribution"
                        />
                    </Grid>
                </>
            );
        case 'fixed':
            return (
                <Grid item xs={12} sm={6}>
                    <TextField
                        fullWidth
                        label="Fixed Demand"
                        type="number"
                        value={config.average_demand}
                        onChange={handleInputChange('average_demand')}
                        helperText="Constant demand value"
                    />
                </Grid>
            );
        default:
            return null;
    }
}

// Add proper type for zoomState
interface ZoomState {
    left: number | 'dataMin';
    right: number | 'dataMax';
    refAreaLeft: string | number;
    refAreaRight: string | number;
    top: number | 'dataMax+1';
    bottom: number | 'dataMin-1';
    animation: boolean;
}

// Add these interfaces after the existing interfaces
interface ValidationErrors {
    simulation_length?: string;
    average_lead_time?: string;
    average_demand?: string;
    std_demand?: string;
    starting_inventory?: string;
    minimum_lot_size?: string;
    ordering_cost?: string;
    holding_cost?: string;
    purchase_period?: string;
    gamma_shape?: string;
    gamma_scale?: string;
    uniform_min?: string;
    uniform_max?: string;
    seed?: string;
}

// Add these validation functions before the Home component
const validateNumber = (value: number | undefined, fieldName: string, min = 0): string | undefined => {
    if (value === undefined || value === null) {
        return `${fieldName} is required`;
    }
    if (isNaN(Number(value))) {
        return `${fieldName} must be a number`;
    }
    if (Number(value) < min) {
        return `${fieldName} must be greater than ${min}`;
    }
    return undefined;
};

const validateConfig = (config: SimulationConfig): ValidationErrors => {
    const errors: ValidationErrors = {};

    // Basic validations
    errors.simulation_length = validateNumber(config.simulation_length, 'Simulation length', 1);
    errors.average_lead_time = validateNumber(config.average_lead_time, 'Average lead time', 0);
    errors.starting_inventory = validateNumber(config.starting_inventory, 'Starting inventory', 0);
    errors.minimum_lot_size = validateNumber(config.minimum_lot_size, 'Minimum lot size', 1);

    // Distribution-specific validations
    switch (config.demand_distribution) {
        case 'normal':
            errors.average_demand = validateNumber(config.average_demand, 'Average demand', 0);
            errors.std_demand = validateNumber(config.std_demand, 'Standard deviation of demand', 0);
            break;
        case 'gamma':
            errors.gamma_shape = validateNumber(config.gamma_shape, 'Shape parameter', 0);
            errors.gamma_scale = validateNumber(config.gamma_scale, 'Scale parameter', 0);
            break;
        case 'uniform':
            errors.uniform_min = validateNumber(config.uniform_min, 'Minimum demand', 0);
            errors.uniform_max = validateNumber(config.uniform_max, 'Maximum demand');
            if (config.uniform_max && config.uniform_min && config.uniform_max <= config.uniform_min) {
                errors.uniform_max = 'Maximum demand must be greater than minimum demand';
            }
            break;
        case 'poisson':
        case 'fixed':
            errors.average_demand = validateNumber(config.average_demand, 'Average demand', 0);
            break;
    }

    // Policy-specific validations
    if (config.policy === 'EOQ') {
        errors.ordering_cost = validateNumber(config.ordering_cost, 'Ordering cost', 0);
        errors.holding_cost = validateNumber(config.holding_cost, 'Holding cost rate', 0);
    }
    if (config.policy === 'TPOP') {
        errors.purchase_period = validateNumber(config.purchase_period, 'Purchase period', 1);
    }

    return errors;
};

// Add this function before the Home component
const throwConfetti = () => {
    confetti({
        particleCount: 100,
        spread: 70,
        origin: { y: 0.6 }
    });
};

export default function Home() {
    const [config, setConfig] = useState<SimulationConfig>({
        simulation_length: 30,
        average_lead_time: 5,
        average_demand: 50.0,
        std_demand: 10.0,
        policy: 'ROP',
        starting_inventory: 0,
        log_level: 0,
        seed: 7,
        minimum_lot_size: 10,
        deterministic: false,
        demand_distribution: 'normal',
    });

    const [result, setResult] = useState<SimulationResult | null>(null);
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState<string | null>(null);
    const [tabValue, setTabValue] = useState(0);
    const [darkMode, setDarkMode] = useState(false);
    const [generateLogs, setGenerateLogs] = useState(false);
    const [showLoadingOverlay, setShowLoadingOverlay] = useState(false);
    const resultsRef = useRef<HTMLDivElement>(null);

    const [zoomState, setZoomState] = useState<ZoomState>({
        left: 'dataMin',
        right: 'dataMax',
        refAreaLeft: '',
        refAreaRight: '',
        top: 'dataMax+1',
        bottom: 'dataMin-1',
        animation: true,
    });

    // Add validation state
    const [validationErrors, setValidationErrors] = useState<ValidationErrors>({});

    const [showThankYou, setShowThankYou] = useState(false);

    const handleInputChange = (field: keyof SimulationConfig) => (
        event: React.ChangeEvent<HTMLInputElement>
    ) => {
        const value = event.target.value;
        const newConfig = {
            ...config,
            [field]: value === '' ? undefined :
                (field === 'policy' ? value : Number(value))
        };
        setConfig(newConfig);

        // Validate the new value
        const newErrors = validateConfig(newConfig);
        setValidationErrors(newErrors);
    };

    const runSimulation = async (customConfig?: SimulationConfig) => {
        const configToValidate = customConfig || config;
        const errors = validateConfig(configToValidate);
        const hasErrors = Object.values(errors).some(error => error !== undefined);

        if (hasErrors) {
            setValidationErrors(errors);
            setError('Please fix the validation errors before running the simulation.');
            return;
        }

        setShowLoadingOverlay(true);
        setLoading(true);
        setError(null);

        try {
            const configToUse = {
                ...(customConfig || config),
                log_level: generateLogs ? 2 : 0
            };
            const queryParams = new URLSearchParams();
            Object.entries(configToUse).forEach(([key, value]) => {
                if (value !== undefined && value !== null) {
                    if (typeof value === 'boolean') {
                        if (value) queryParams.append(key, '');
                    } else {
                        queryParams.append(key, value.toString());
                    }
                }
            });

            const apiUrl = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:47761';
            const url = `${apiUrl}/simulate?${queryParams.toString()}`;
            console.log('Sending request to:', url); // Debug log

            const response = await fetch(url, {
                method: 'POST',
                headers: {
                    'Accept': 'application/json',
                    'Content-Type': 'application/json',
                },
                mode: 'cors', // Enable CORS
                cache: 'no-cache',
                credentials: 'same-origin',
            });

            if (!response.ok) {
                const errorText = await response.text();
                console.error('Server response:', response.status, errorText); // Debug log
                throw new Error(`Server error: ${response.status} - ${errorText}`);
            }

            const data = await response.json();
            console.log('Received data:', data); // Debug log
            setResult(data);

            // Add artificial delay for better UX
            await new Promise(resolve => setTimeout(resolve, 2000));

            // After simulation completes
            setShowLoadingOverlay(false);

            // Scroll to results
            setTimeout(() => {
                resultsRef.current?.scrollIntoView({ behavior: 'smooth', block: 'start' });
            }, 100);

        } catch (err) {
            console.error('Simulation error:', err); // Debug log
            if (err instanceof TypeError && err.message === 'Failed to fetch') {
                setError('Could not connect to the simulation server. Please ensure the server is running on port 47761.');
            } else {
                setError(err instanceof Error ? err.message : 'An unexpected error occurred');
            }
        } finally {
            setLoading(false);
            setShowLoadingOverlay(false);
        }
    };

    const handleTabChange = (event: React.SyntheticEvent, newValue: number) => {
        setTabValue(newValue);
    };

    const getPlotData = () => {
        if (!result) return [];
        return result.inventory_quantity.map((_, index) => ({
            day: index,
            inventory: result.inventory_quantity[index],
            demand: result.demand_quantity[index],
            procurement: result.procurement_quantity[index],
            purchase: result.purchase_quantity[index],
            sales: result.sale_quantity[index],
            lostSales: result.lost_sale_quantity[index],
        }));
    };

    const calculateAnalysis = (data: SimulationResult): AnalysisReport => {
        const inventoryArr = data.inventory_quantity;
        const demandArr = data.demand_quantity;
        const purchaseArr = data.purchase_quantity;
        const salesArr = data.sale_quantity;
        const lostSalesArr = data.lost_sale_quantity;

        const totalDemand = demandArr.reduce((a, b) => a + b, 0);
        const totalSales = salesArr.reduce((a, b) => a + b, 0);
        const totalLostSales = lostSalesArr.reduce((a, b) => a + b, 0);
        const totalPurchases = purchaseArr.reduce((a, b) => a + b, 0);

        const orders = purchaseArr.filter(x => x > 0);
        const averageInventory = inventoryArr.reduce((a, b) => a + b, 0) / inventoryArr.length;

        return {
            serviceLevelPct: ((totalSales / (totalSales + totalLostSales)) * 100) || 0,
            averageInventory: averageInventory,
            totalLostSales: totalLostSales,
            totalPurchases: totalPurchases,
            averageDailyDemand: totalDemand / demandArr.length,
            totalSales: totalSales,
            inventoryTurns: (totalSales / averageInventory) || 0,
            peakInventory: Math.max(...inventoryArr),
            minimumInventory: Math.min(...inventoryArr),
            numberOfOrders: orders.length,
            averageOrderSize: orders.length ? totalPurchases / orders.length : 0,
            averageOrderInterval: orders.length > 1 ? purchaseArr.length / (orders.length - 1) : 0,
            totalOrderQuantity: totalPurchases
        };
    };

    const downloadCSV = () => {
        if (!result) return;

        const headers = ['Day', 'Inventory', 'Demand', 'Procurement', 'Purchase', 'Sales', 'Lost Sales'];
        const rows = result.inventory_quantity.map((_, index) => [
            index,
            result.inventory_quantity[index],
            result.demand_quantity[index],
            result.procurement_quantity[index],
            result.purchase_quantity[index],
            result.sale_quantity[index],
            result.lost_sale_quantity[index]
        ]);

        const csvContent = [
            headers.join(','),
            ...rows.map(row => row.join(','))
        ].join('\n');

        const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
        const link = document.createElement('a');
        const url = URL.createObjectURL(blob);
        link.setAttribute('href', url);
        link.setAttribute('download', 'simulation_results.csv');
        link.style.visibility = 'hidden';
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
    };

    const handleZoom = (direction: 'in' | 'out') => {
        if (direction === 'out') {
            setZoomState(prev => ({
                ...prev,
                left: 'dataMin',
                right: 'dataMax',
                top: 'dataMax+1',
                bottom: 'dataMin-1',
            }));
        }
    };

    const handleChartMouseDown = (e: any) => {
        if (e && e.activeLabel) {
            setZoomState(prev => ({ ...prev, refAreaLeft: e.activeLabel }));
        }
    };

    const handleChartMouseMove = (e: any) => {
        if (zoomState.refAreaLeft && e && e.activeLabel) {
            setZoomState(prev => ({ ...prev, refAreaRight: e.activeLabel }));
        }
    };

    const handleChartMouseUp = () => {
        if (zoomState.refAreaLeft && zoomState.refAreaRight) {
            const left = Math.min(Number(zoomState.refAreaLeft), Number(zoomState.refAreaRight));
            const right = Math.max(Number(zoomState.refAreaLeft), Number(zoomState.refAreaRight));

            setZoomState(prev => ({
                ...prev,
                refAreaLeft: '',
                refAreaRight: '',
                left,
                right,
            }));
        }
    };

    return (
        <ThemeProvider theme={customTheme(darkMode ? 'dark' : 'light')}>
            <Box sx={{
                minHeight: '100vh',
                bgcolor: 'background.default',
                color: 'text.primary',
                transition: 'all 0.3s ease',
            }}>
                <SimulationLoadingOverlay isVisible={showLoadingOverlay} />
                <ThankYouModal
                    isVisible={showThankYou}
                    onClose={() => setShowThankYou(false)}
                />
                <Head>
                    <title>ChainSim - Supply Chain Simulator</title>
                    <meta name="description" content="Supply Chain Simulation Tool" />
                </Head>

                <IconButton
                    sx={{ position: 'fixed', right: 20, top: 20, zIndex: 1000 }}
                    onClick={() => setDarkMode(!darkMode)}
                    color="inherit"
                >
                    {darkMode ? <Brightness7Icon /> : <Brightness4Icon />}
                </IconButton>

                <Container maxWidth="lg" sx={{ py: 6 }}>
                    {/* Header */}
                    <Typography
                        variant="h1"
                        component="h1"
                        sx={{
                            fontFamily: 'Ubuntu',
                            fontSize: '2.5rem',
                            mb: 2,
                            textAlign: 'center',
                        }}
                    >
                        ChainSim - The Supply Chain Simulator
                    </Typography>

                    {/* Description */}
                    <Card sx={{
                        mb: 4,
                        border: theme => `2px solid ${theme.palette.mode === 'dark' ? '#333' : '#e0e0e0'}`,
                        borderRadius: 2,
                        transition: 'all 0.3s ease',
                        '&:hover': {
                            transform: 'translateY(-2px)',
                            boxShadow: theme => theme.palette.mode === 'dark' ? '0 8px 16px rgba(0,0,0,0.4)' : '0 8px 16px rgba(0,0,0,0.1)',
                        }
                    }}>
                        <CardContent>
                            <Typography variant="h5" gutterBottom>
                                About ChainSim
                            </Typography>
                            <Typography variant="body1" paragraph>
                                ChainSim is a powerful supply chain simulation tool that helps analyze
                                different inventory management policies and their effects on supply chain
                                performance.
                            </Typography>
                            <Typography variant="h6" gutterBottom>
                                Features:
                            </Typography>
                            <ul>
                                <Typography component="li">
                                    Multiple inventory policies (ROP, EOQ, TPOP)
                                </Typography>
                                <Typography component="li">
                                    Detailed simulation of demand and procurement
                                </Typography>
                                <Typography component="li">
                                    Comprehensive performance metrics
                                </Typography>
                                <Typography component="li">
                                    Real-time simulation results
                                </Typography>
                            </ul>
                        </CardContent>
                    </Card>

                    {/* Configuration Form */}
                    <Card className="major-card" sx={{
                        mb: 4,
                        position: 'relative',
                        '&::before': {
                            content: '""',
                            position: 'absolute',
                            top: 0,
                            left: 0,
                            right: 0,
                            height: '4px',
                            background: theme => `linear-gradient(90deg, ${theme.palette.primary.main}, ${alpha(theme.palette.primary.main, 0.5)})`,
                            borderTopLeftRadius: '16px',
                            borderTopRightRadius: '16px',
                        },
                    }}>
                        <CardContent>
                            <Typography variant="h5" gutterBottom>
                                Simulation Configuration
                            </Typography>
                            <Grid container spacing={3}>
                                <Grid item xs={12} sm={6}>
                                    <PolicySelector
                                        value={config.policy}
                                        onChange={handleInputChange('policy')}
                                    />
                                </Grid>

                                <Grid item xs={12} sm={6}>
                                    <TextField
                                        fullWidth
                                        label="Simulation Length (days)"
                                        type="number"
                                        value={config.simulation_length}
                                        onChange={handleInputChange('simulation_length')}
                                        error={!!validationErrors.simulation_length}
                                        helperText={validationErrors.simulation_length}
                                        required
                                    />
                                </Grid>

                                <Grid item xs={12} sm={6}>
                                    <TextField
                                        fullWidth
                                        label="Average Lead Time"
                                        type="number"
                                        value={config.average_lead_time}
                                        onChange={handleInputChange('average_lead_time')}
                                        error={!!validationErrors.average_lead_time}
                                        helperText={validationErrors.average_lead_time}
                                        required
                                    />
                                </Grid>

                                <Grid item xs={12} sm={6}>
                                    <TextField
                                        fullWidth
                                        label="Seed"
                                        type="number"
                                        value={config.seed}
                                        onChange={handleInputChange('seed')}
                                        error={!!validationErrors.seed}
                                        helperText={validationErrors.seed}
                                        required
                                    />
                                </Grid>
                                <Grid item xs={12} sm={6}>
                                    <TextField
                                        fullWidth
                                        label="Minimum Lot Size"
                                        type="number"
                                        value={config.minimum_lot_size}
                                        onChange={handleInputChange('minimum_lot_size')}
                                        error={!!validationErrors.minimum_lot_size}
                                        helperText={validationErrors.minimum_lot_size}
                                        required
                                    />
                                </Grid>
                                <Grid item xs={12} sm={6}>
                                    <TextField
                                        fullWidth
                                        label="Starting Inventory"
                                        type="number"
                                        value={config.starting_inventory}
                                        onChange={handleInputChange('starting_inventory')}
                                        error={!!validationErrors.starting_inventory}
                                        helperText={validationErrors.starting_inventory}
                                        required
                                    />
                                </Grid>
                                <Grid item xs={12} sm={6}>
                                    <FormControlLabel
                                        control={
                                            <Switch
                                                checked={config.deterministic}
                                                onChange={(e) => setConfig({
                                                    ...config,
                                                    deterministic: e.target.checked
                                                })}
                                            />
                                        }
                                        label="Deterministic Mode"
                                    />
                                </Grid>

                                {/* Policy-specific fields */}
                                {config.policy === 'EOQ' && (
                                    <>
                                        <Grid item xs={12} sm={6}>
                                            <TextField
                                                fullWidth
                                                label="Ordering Cost"
                                                type="number"
                                                value={config.ordering_cost}
                                                onChange={handleInputChange('ordering_cost')}
                                                error={!!validationErrors.ordering_cost}
                                                helperText={validationErrors.ordering_cost}
                                                required
                                            />
                                        </Grid>
                                        <Grid item xs={12} sm={6}>
                                            <TextField
                                                fullWidth
                                                label="Holding Cost Rate"
                                                type="number"
                                                value={config.holding_cost}
                                                onChange={handleInputChange('holding_cost')}
                                                error={!!validationErrors.holding_cost}
                                                helperText={validationErrors.holding_cost}
                                                required
                                            />
                                        </Grid>
                                    </>
                                )}

                                {config.policy === 'TPOP' && (
                                    <Grid item xs={12} sm={6}>
                                        <TextField
                                            fullWidth
                                            label="Purchase Period"
                                            type="number"
                                            value={config.purchase_period}
                                            onChange={handleInputChange('purchase_period')}
                                            error={!!validationErrors.purchase_period}
                                            helperText={validationErrors.purchase_period}
                                            required
                                        />
                                    </Grid>
                                )}

                                <Grid item xs={12} sm={6}>
                                    <FormControlLabel
                                        control={
                                            <Switch
                                                checked={generateLogs}
                                                onChange={(e) => setGenerateLogs(e.target.checked)}
                                            />
                                        }
                                        label="Generate Logs on Server"
                                    />
                                </Grid>

                                <Grid item xs={12} sm={6}>
                                    <TextField
                                        fullWidth
                                        select
                                        label="Demand Distribution"
                                        value={config.demand_distribution}
                                        onChange={(e) => setConfig({
                                            ...config,
                                            demand_distribution: e.target.value as SimulationConfig['demand_distribution']
                                        })}
                                        helperText="Select the probability distribution for demand"
                                    >
                                        <MenuItem value="fixed">Fixed (Constant)</MenuItem>
                                        <MenuItem value="normal">Normal</MenuItem>
                                        <MenuItem value="gamma">Gamma</MenuItem>
                                        <MenuItem value="poisson">Poisson</MenuItem>
                                        <MenuItem value="uniform">Uniform</MenuItem>
                                    </TextField>
                                </Grid>

                                <DemandDistributionParams
                                    config={config}
                                    handleInputChange={handleInputChange}
                                    validationErrors={validationErrors}
                                />

                                <Grid item xs={12}>
                                    <Box sx={{ display: 'flex', gap: 2 }}>
                                        <Button
                                            variant="contained"
                                            color="primary"
                                            onClick={() => runSimulation()}
                                            disabled={loading}
                                            fullWidth
                                            startIcon={loading && <CircularProgress size={20} color="inherit" />}
                                            sx={{ flex: 1 }}
                                        >
                                            {loading ? 'Running Simulation...' : 'Run Simulation'}
                                        </Button>
                                        <Button
                                            variant="outlined"
                                            color="secondary"
                                            onClick={() => {
                                                throwConfetti();
                                                setShowThankYou(true);
                                                setTimeout(() => {
                                                    setShowThankYou(false);
                                                }, 2000);
                                            }}
                                            startIcon={<span role="img" aria-label="party">🎉</span>}
                                        >
                                            Is this cool?
                                        </Button>
                                    </Box>
                                </Grid>
                            </Grid>
                        </CardContent>
                    </Card>

                    {/* Error Display */}
                    {error && (
                        <Alert
                            severity="error"
                            sx={{ mb: 4 }}
                            action={
                                <Button
                                    color="inherit"
                                    size="small"
                                    onClick={() => setError(null)}
                                >
                                    Dismiss
                                </Button>
                            }
                        >
                            {error}
                        </Alert>
                    )}
                    {/* Add ref to results section */}
                    <div ref={resultsRef}>
                        {result && (
                            <motion.div
                                initial={{ opacity: 0, y: 20 }}
                                animate={{ opacity: 1, y: 0 }}
                                transition={{ duration: 0.5 }}
                            >
                                <Card className="major-card" sx={{ mb: 4, p: 2 }}>
                                    <CardContent sx={{ p: 3 }}>
                                        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
                                            <Typography variant="h5">
                                                Simulation Results Analysis
                                            </Typography>
                                            <Button
                                                variant="outlined"
                                                onClick={downloadCSV}
                                                startIcon={<DownloadIcon />}
                                            >
                                                Download CSV
                                            </Button>
                                        </Box>
                                        <Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
                                            <Tabs
                                                value={tabValue}
                                                onChange={handleTabChange}
                                                variant="scrollable"
                                                scrollButtons="auto"
                                            >
                                                <Tab label="Inventory" />
                                                <Tab label="Demand & Sales" />
                                                <Tab label="Procurement" />
                                                <Tab label="Lost Sales" />
                                                <Tab label="Combined View" />
                                            </Tabs>
                                        </Box>

                                        <TabPanel value={tabValue} index={0}>
                                            <ZoomableChart
                                                data={getPlotData()}
                                                series={[
                                                    { key: 'inventory', color: '#8884d8' },
                                                    { key: 'demand', color: '#82ca9d' },
                                                    // ... other series
                                                ]}
                                                zoomState={zoomState}
                                                onChartMouseDown={handleChartMouseDown}
                                                onChartMouseMove={handleChartMouseMove}
                                                onChartMouseUp={handleChartMouseUp}
                                            />
                                        </TabPanel>

                                        <TabPanel value={tabValue} index={1}>
                                            <ZoomableChart
                                                data={getPlotData()}
                                                series={[
                                                    { key: 'demand', color: '#82ca9d' },
                                                    { key: 'sales', color: '#ffc658' },
                                                    // ... other series
                                                ]}
                                                zoomState={zoomState}
                                                onChartMouseDown={handleChartMouseDown}
                                                onChartMouseMove={handleChartMouseMove}
                                                onChartMouseUp={handleChartMouseUp}
                                            />
                                        </TabPanel>

                                        <TabPanel value={tabValue} index={2}>
                                            <ZoomableChart
                                                data={getPlotData()}
                                                series={[
                                                    { key: 'procurement', color: '#ff7300' },
                                                    { key: 'purchase', color: '#ff0000' },
                                                    // ... other series
                                                ]}
                                                zoomState={zoomState}
                                                onChartMouseDown={handleChartMouseDown}
                                                onChartMouseMove={handleChartMouseMove}
                                                onChartMouseUp={handleChartMouseUp}
                                            />
                                        </TabPanel>

                                        <TabPanel value={tabValue} index={3}>
                                            <ZoomableChart
                                                data={getPlotData()}
                                                series={[
                                                    { key: 'lostSales', color: '#ff0000' },
                                                    // ... other series
                                                ]}
                                                zoomState={zoomState}
                                                onChartMouseDown={handleChartMouseDown}
                                                onChartMouseMove={handleChartMouseMove}
                                                onChartMouseUp={handleChartMouseUp}
                                            />
                                        </TabPanel>

                                        <TabPanel value={tabValue} index={4}>
                                            <ZoomableChart
                                                data={getPlotData()}
                                                series={[
                                                    { key: 'inventory', color: '#8884d8' },
                                                    { key: 'demand', color: '#82ca9d' },
                                                    { key: 'procurement', color: '#ff7300' },
                                                    { key: 'purchase', color: '#ff0000' },
                                                    { key: 'sales', color: '#ffc658' },
                                                    { key: 'lostSales', color: '#ff0000' },
                                                    // ... other series
                                                ]}
                                                zoomState={zoomState}
                                                onChartMouseDown={handleChartMouseDown}
                                                onChartMouseMove={handleChartMouseMove}
                                                onChartMouseUp={handleChartMouseUp}
                                            />
                                        </TabPanel>
                                    </CardContent>
                                </Card>

                                {/* Analysis section - place this right after the graphs card */}
                                <Card className="major-card" sx={{ mb: 4 }}>
                                    <CardContent>
                                        <Typography variant="h5" gutterBottom>
                                            Simulation Analysis Report
                                        </Typography>
                                        <>
                                            <Grid container spacing={3}>
                                                <Grid item xs={12} md={6}>
                                                    <Typography variant="h6" gutterBottom>
                                                        Performance Metrics
                                                    </Typography>
                                                    {getStatisticsMetadata(calculateAnalysis(result)).slice(0, 6).map((stat, index) => (
                                                        <StatisticCard key={index} statistic={stat} />
                                                    ))}
                                                </Grid>
                                                <Grid item xs={12} md={6}>
                                                    <Typography variant="h6" gutterBottom>
                                                        Order Analysis
                                                    </Typography>
                                                    {getStatisticsMetadata(calculateAnalysis(result)).slice(6).map((stat, index) => (
                                                        <StatisticCard key={index} statistic={stat} />
                                                    ))}
                                                </Grid>
                                            </Grid>
                                        </>
                                    </CardContent>
                                </Card>

                                {/* Existing table view */}
                                <Card className="major-card">
                                    <CardContent>
                                        <Typography variant="h5" gutterBottom>
                                            Detailed Results Table
                                        </Typography>
                                        <TableContainer component={Paper}>
                                            <Table>
                                                <TableHead>
                                                    <TableRow>
                                                        <TableCell>Day</TableCell>
                                                        <TableCell>Inventory</TableCell>
                                                        <TableCell>Demand</TableCell>
                                                        <TableCell>Procurement</TableCell>
                                                        <TableCell>Purchase</TableCell>
                                                        <TableCell>Sales</TableCell>
                                                        <TableCell>Lost Sales</TableCell>
                                                    </TableRow>
                                                </TableHead>
                                                <TableBody>
                                                    {result.inventory_quantity.map((_, index) => (
                                                        <TableRow key={index}>
                                                            <TableCell>{index}</TableCell>
                                                            <TableCell>{result.inventory_quantity[index]}</TableCell>
                                                            <TableCell>{result.demand_quantity[index]}</TableCell>
                                                            <TableCell>{result.procurement_quantity[index]}</TableCell>
                                                            <TableCell>{result.purchase_quantity[index]}</TableCell>
                                                            <TableCell>{result.sale_quantity[index]}</TableCell>
                                                            <TableCell>{result.lost_sale_quantity[index]}</TableCell>
                                                        </TableRow>
                                                    ))}
                                                </TableBody>
                                            </Table>
                                        </TableContainer>
                                    </CardContent>
                                </Card>
                            </motion.div>
                        )}
                    </div>
                </Container>

                <ScrollButtons />
            </Box>
        </ThemeProvider>
    );
} 