import {
    LineChart, Line, XAxis, YAxis, CartesianGrid,
    Tooltip, Legend, ResponsiveContainer, ReferenceArea
} from 'recharts';
import { Box, Button } from '@mui/material';
import ZoomOutIcon from '@mui/icons-material/ZoomOut';
import ZoomInIcon from '@mui/icons-material/ZoomIn';

interface ZoomableChartProps {
    data: any[];
    series: Array<{
        key: string;
        color: string;
    }>;
    zoomState: any;
    onZoom: (direction: 'in' | 'out') => void;
    onChartMouseDown: (e: any) => void;
    onChartMouseMove: (e: any) => void;
    onChartMouseUp: () => void;
}

export const ZoomableChart = ({
    data,
    series,
    zoomState,
    onZoom,
    onChartMouseDown,
    onChartMouseMove,
    onChartMouseUp
}: ZoomableChartProps) => {
    return (
        <>
            <ResponsiveContainer width="100%" height={400}>
                <LineChart
                    data={data}
                    onMouseDown={onChartMouseDown}
                    onMouseMove={onChartMouseMove}
                    onMouseUp={onChartMouseUp}
                >
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis
                        dataKey="day"
                        domain={[zoomState.left, zoomState.right]}
                        type="number"
                    />
                    <YAxis
                        domain={[zoomState.bottom, zoomState.top]}
                        type="number"
                    />
                    <Tooltip />
                    <Legend />
                    {series.map(({ key, color }) => (
                        <Line
                            key={key}
                            type="monotone"
                            dataKey={key}
                            stroke={color}
                            activeDot={{ r: 8 }}
                        />
                    ))}
                    {zoomState.refAreaLeft && zoomState.refAreaRight ? (
                        <ReferenceArea
                            x1={zoomState.refAreaLeft}
                            x2={zoomState.refAreaRight}
                            strokeOpacity={0.3}
                        />
                    ) : null}
                </LineChart>
            </ResponsiveContainer>
            <Box sx={{ mt: 2, display: 'flex', justifyContent: 'center', gap: 2 }}>
                <Button
                    variant="outlined"
                    size="small"
                    onClick={() => onZoom('in')}
                    startIcon={<ZoomInIcon />}
                >
                    Zoom In
                </Button>
                <Button
                    variant="outlined"
                    size="small"
                    onClick={() => onZoom('out')}
                    startIcon={<ZoomOutIcon />}
                >
                    Reset Zoom
                </Button>
            </Box>
        </>
    );
}; 