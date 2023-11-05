
#ifndef CHAINSIM_CHAINLOGGER_HPP
#define CHAINSIM_CHAINLOGGER_HPP

#include <iostream>

class ChainLogger {
    unsigned m_logging_level{};

public:
    explicit ChainLogger(unsigned loggingLevel = 1) : m_logging_level{loggingLevel} {}

    template<typename... Args>
    void info(const Args &... args) const {
        if (m_logging_level < 1)
            return;

        std::cout << "[INFO > ] ";
        ((std::cout << args), ...);

        std::cout << "\n";
    }

    template<typename... Args>
    void warn(const Args &... args) const {
        if (m_logging_level < 1)
            return;

        std::cout << "[WARN ! ] ";
        ((std::cout << args), ...);

        std::cout << "\n";
    }

    template<typename... Args>
    void error(const Args &... args) const {
        if (m_logging_level < 1)
            return;

        std::cout << "[ERROR !!] ";
        ((std::cout << args), ...);

        std::cout << "\n";
    }
};

#endif //CHAINSIM_CHAINLOGGER_HPP
