export interface SimulationConfig {
    simulation_length: number;
    average_lead_time: number;
    average_demand: number;
    std_demand: number;
    policy: 'ROP' | 'EOQ' | 'TPOP';
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

export interface ValidationErrors {
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

export interface SimulationResult {
    inventory_quantity: number[];
    demand_quantity: number[];
    procurement_quantity: number[];
    purchase_quantity: number[];
    sale_quantity: number[];
    lost_sale_quantity: number[];
} 