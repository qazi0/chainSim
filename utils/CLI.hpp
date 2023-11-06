
#ifndef CHAINSIM_CLI_HPP
#define CHAINSIM_CLI_HPP

#include "../third_party/argparse.hpp"

namespace qz {

    void parse_command_line_args(argparse::ArgumentParser &parser, int argc, char *argv[]) {
        parser.add_argument("--log_level")
                .default_value(0u)
                .scan<'u', unsigned>()
                .help("Logging level (0-2)");

        parser.add_argument("--simulation_length")
                .default_value(30ULL)
                .scan<'u', uint64_t>()
                .help("Length of the simulation");

        parser.add_argument("--average_demand")
                .default_value(50.0)
                .scan<'g', double>()
                .help("Average of demand (per time unit)");

        parser.add_argument("--std_demand")
                .default_value(10.0)
                .scan<'g', double>()
                .help("Standard deviation of demand (per time unit)");

        parser.add_argument("--average_lead_time")
                .default_value(5ULL)
                .scan<'u', uint64_t>()
                .help("Average lead time");

        parser.add_argument("--purchase_period")
                .default_value(7ULL)
                .scan<'u', uint64_t>()
                .help("Purchase period (applies to Periodic purchase policies)");

        parser.add_argument("--minimum_lot_size")
                .default_value(10ULL)
                .scan<'u', uint64_t>()
                .help("Minimum lot size (quantity procure-able from a supplier)");

        parser.add_argument("--seed")
                .default_value(7)
                .scan<'i', int>()
                .help("PRNG seed, for demand/lead time generators.");

        parser.add_argument("--starting_inventory")
                .default_value(0ULL)
                .scan<'u', uint64_t>()
                .help("Starting inventory for the simulation");

        parser.add_argument("--output_file")
                .default_value(std::string{"simulation_records.csv"})
                .help("Path/name of the output records file (.csv)");

        parser.add_argument("--policy")
                .default_value(std::string{"ROP"})
                .help("Purchasing policy: (ROP | TPOP | EOQ | EBQ | CUSTOM)");

        parser.add_argument("--deterministic")
                .default_value(false)
                .implicit_value(true)
                .help("Simulate without random sampling, use fixed leadtime/demand/etc.");

        try {
            parser.parse_args(argc, argv);
        }
        catch (const std::runtime_error &err) {
            std::cerr << err.what() << std::endl;
            std::cerr << parser;
            std::exit(1);
        }
    }
}

#endif //CHAINSIM_CLI_HPP
