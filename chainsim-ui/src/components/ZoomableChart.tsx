import React from 'react';
import {
    LineChart, Line, XAxis, YAxis, CartesianGrid,
    Tooltip, Legend, ResponsiveContainer, ReferenceArea
} from 'recharts';

interface ZoomState {
    left: number | 'dataMin';
    right: number | 'dataMax';
    refAreaLeft: string | number;
    refAreaRight: string | number;
    top: number | 'dataMax+1';
    bottom: number | 'dataMin-1';
    animation: boolean;
}

interface ZoomableChartProps {
    data: any[];
    series: Array<{
        key: string;
        color: string;
    }>;
    zoomState: ZoomState;
    onChartMouseDown: (e: any) => void;
    onChartMouseMove: (e: any) => void;
    onChartMouseUp: () => void;
}

export const ZoomableChart: React.FC<ZoomableChartProps> = ({
    data,
    series,
    zoomState,
    onChartMouseDown,
    onChartMouseMove,
    onChartMouseUp
}) => {
    return (
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
    );
}; 