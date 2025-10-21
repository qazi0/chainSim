import { TextField, MenuItem } from '@mui/material';
import { SimulationConfig } from '../types';

interface PolicySelectorProps {
    value: SimulationConfig['policy'];
    onChange: (event: React.ChangeEvent<HTMLInputElement>) => void;
}

export const PolicySelector = ({ value, onChange }: PolicySelectorProps) => {
    return (
        <TextField
            fullWidth
            select
            label="Inventory Policy"
            value={value}
            onChange={onChange}
        >
            <MenuItem value="ROP">Reorder Point Policy (ROP)</MenuItem>
            <MenuItem value="EOQ">Economic Order Quantity (EOQ)</MenuItem>
            <MenuItem value="TPOP">Time-Phased Order Point (TPOP)</MenuItem>
        </TextField>
    );
}; 