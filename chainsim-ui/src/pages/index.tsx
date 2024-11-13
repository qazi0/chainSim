import { useState } from 'react';
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
} from 'recharts';
import '@fontsource/ubuntu/700.css';
import '@fontsource/roboto/400.css';

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

export default function Home() {
    const [config, setConfig] = useState<SimulationConfig>({
        simulation_length: 30,
        average_lead_time: 5,
        average_demand: 50.0,
        std_demand: 10.0,
        policy: 'ROP',
        starting_inventory: 0,
        log_level: 2,
        seed: 7,
        minimum_lot_size: 10,
        deterministic: false
    });

    const [result, setResult] = useState<SimulationResult | null>(null);
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState<string | null>(null);
    const [tabValue, setTabValue] = useState(0);

    const handleInputChange = (field: keyof SimulationConfig) => (
        event: React.ChangeEvent<HTMLInputElement>
    ) => {
        setConfig({
            ...config,
            [field]: event.target.value,
        });
    };

    const runSimulation = async () => {
        setLoading(true);
        setError(null);

        try {
            const queryParams = new URLSearchParams();
            Object.entries(config).forEach(([key, value]) => {
                if (value !== undefined && value !== null) {
                    if (typeof value === 'boolean') {
                        if (value) queryParams.append(key, '');
                    } else {
                        queryParams.append(key, value.toString());
                    }
                }
            });

            const url = `http://localhost:47761/simulate?${queryParams.toString()}`;
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
        } catch (err) {
            console.error('Simulation error:', err); // Debug log
            if (err instanceof TypeError && err.message === 'Failed to fetch') {
                setError('Could not connect to the simulation server. Please ensure the server is running on port 47761.');
            } else {
                setError(err instanceof Error ? err.message : 'An unexpected error occurred');
            }
        } finally {
            setLoading(false);
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

    return (
        <>
            <Head>
                <title>ChainSim - Supply Chain Simulator</title>
                <meta name="description" content="Supply Chain Simulation Tool" />
            </Head>

            <Container maxWidth="lg" sx={{ py: 4 }}>
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
                <Card sx={{ mb: 4 }}>
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
                <Card sx={{ mb: 4 }}>
                    <CardContent>
                        <Typography variant="h5" gutterBottom>
                            Simulation Configuration
                        </Typography>
                        <Grid container spacing={3}>
                            <Grid item xs={12} sm={6}>
                                <TextField
                                    fullWidth
                                    label="Simulation Length (days)"
                                    type="number"
                                    value={config.simulation_length}
                                    onChange={handleInputChange('simulation_length')}
                                />
                            </Grid>
                            <Grid item xs={12} sm={6}>
                                <TextField
                                    fullWidth
                                    label="Average Lead Time"
                                    type="number"
                                    value={config.average_lead_time}
                                    onChange={handleInputChange('average_lead_time')}
                                />
                            </Grid>
                            <Grid item xs={12} sm={6}>
                                <TextField
                                    fullWidth
                                    label="Average Demand"
                                    type="number"
                                    value={config.average_demand}
                                    onChange={handleInputChange('average_demand')}
                                />
                            </Grid>
                            <Grid item xs={12} sm={6}>
                                <TextField
                                    fullWidth
                                    label="Standard Deviation of Demand"
                                    type="number"
                                    value={config.std_demand}
                                    onChange={handleInputChange('std_demand')}
                                />
                            </Grid>
                            <Grid item xs={12} sm={6}>
                                <TextField
                                    fullWidth
                                    label="Seed"
                                    type="number"
                                    value={config.seed}
                                    onChange={handleInputChange('seed')}
                                />
                            </Grid>
                            <Grid item xs={12} sm={6}>
                                <TextField
                                    fullWidth
                                    label="Minimum Lot Size"
                                    type="number"
                                    value={config.minimum_lot_size}
                                    onChange={handleInputChange('minimum_lot_size')}
                                />
                            </Grid>
                            <Grid item xs={12} sm={6}>
                                <TextField
                                    fullWidth
                                    label="Starting Inventory"
                                    type="number"
                                    value={config.starting_inventory}
                                    onChange={handleInputChange('starting_inventory')}
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
                            <Grid item xs={12} sm={6}>
                                <TextField
                                    fullWidth
                                    select
                                    label="Policy"
                                    value={config.policy}
                                    onChange={handleInputChange('policy')}
                                >
                                    <MenuItem value="ROP">ROP</MenuItem>
                                    <MenuItem value="EOQ">EOQ</MenuItem>
                                    <MenuItem value="TPOP">TPOP</MenuItem>
                                </TextField>
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
                                        />
                                    </Grid>
                                    <Grid item xs={12} sm={6}>
                                        <TextField
                                            fullWidth
                                            label="Holding Cost Rate"
                                            type="number"
                                            value={config.holding_cost}
                                            onChange={handleInputChange('holding_cost')}
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
                                    />
                                </Grid>
                            )}

                            <Grid item xs={12}>
                                <Button
                                    variant="contained"
                                    color="primary"
                                    onClick={runSimulation}
                                    disabled={loading}
                                    fullWidth
                                    startIcon={loading && <CircularProgress size={20} color="inherit" />}
                                >
                                    {loading ? 'Running Simulation...' : 'Run Simulation'}
                                </Button>
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

                {result && (
                    <>
                        <Card sx={{ mb: 4 }}>
                            <CardContent>
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
                                    <ResponsiveContainer width="100%" height={400}>
                                        <LineChart data={getPlotData()}>
                                            <CartesianGrid strokeDasharray="3 3" />
                                            <XAxis dataKey="day" />
                                            <YAxis />
                                            <Tooltip />
                                            <Legend />
                                            <Line
                                                type="monotone"
                                                dataKey="inventory"
                                                stroke="#8884d8"
                                                activeDot={{ r: 8 }}
                                            />
                                        </LineChart>
                                    </ResponsiveContainer>
                                </TabPanel>

                                <TabPanel value={tabValue} index={1}>
                                    <ResponsiveContainer width="100%" height={400}>
                                        <LineChart data={getPlotData()}>
                                            <CartesianGrid strokeDasharray="3 3" />
                                            <XAxis dataKey="day" />
                                            <YAxis />
                                            <Tooltip />
                                            <Legend />
                                            <Line
                                                type="monotone"
                                                dataKey="demand"
                                                stroke="#82ca9d"
                                                activeDot={{ r: 8 }}
                                            />
                                            <Line
                                                type="monotone"
                                                dataKey="sales"
                                                stroke="#ffc658"
                                                activeDot={{ r: 8 }}
                                            />
                                        </LineChart>
                                    </ResponsiveContainer>
                                </TabPanel>

                                <TabPanel value={tabValue} index={2}>
                                    <ResponsiveContainer width="100%" height={400}>
                                        <LineChart data={getPlotData()}>
                                            <CartesianGrid strokeDasharray="3 3" />
                                            <XAxis dataKey="day" />
                                            <YAxis />
                                            <Tooltip />
                                            <Legend />
                                            <Line
                                                type="monotone"
                                                dataKey="procurement"
                                                stroke="#ff7300"
                                                activeDot={{ r: 8 }}
                                            />
                                            <Line
                                                type="monotone"
                                                dataKey="purchase"
                                                stroke="#ff0000"
                                                activeDot={{ r: 8 }}
                                            />
                                        </LineChart>
                                    </ResponsiveContainer>
                                </TabPanel>

                                <TabPanel value={tabValue} index={3}>
                                    <ResponsiveContainer width="100%" height={400}>
                                        <LineChart data={getPlotData()}>
                                            <CartesianGrid strokeDasharray="3 3" />
                                            <XAxis dataKey="day" />
                                            <YAxis />
                                            <Tooltip />
                                            <Legend />
                                            <Line
                                                type="monotone"
                                                dataKey="lostSales"
                                                stroke="#ff0000"
                                                activeDot={{ r: 8 }}
                                            />
                                        </LineChart>
                                    </ResponsiveContainer>
                                </TabPanel>

                                <TabPanel value={tabValue} index={4}>
                                    <ResponsiveContainer width="100%" height={400}>
                                        <LineChart data={getPlotData()}>
                                            <CartesianGrid strokeDasharray="3 3" />
                                            <XAxis dataKey="day" />
                                            <YAxis />
                                            <Tooltip />
                                            <Legend />
                                            <Line type="monotone" dataKey="inventory" stroke="#8884d8" />
                                            <Line type="monotone" dataKey="demand" stroke="#82ca9d" />
                                            <Line type="monotone" dataKey="procurement" stroke="#ff7300" />
                                            <Line type="monotone" dataKey="purchase" stroke="#ff0000" />
                                            <Line type="monotone" dataKey="sales" stroke="#ffc658" />
                                            <Line type="monotone" dataKey="lostSales" stroke="#ff0000" />
                                        </LineChart>
                                    </ResponsiveContainer>
                                </TabPanel>
                            </CardContent>
                        </Card>

                        {/* Existing table view */}
                        <Card>
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
                    </>
                )}
            </Container>
        </>
    );
} 