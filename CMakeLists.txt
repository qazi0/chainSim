cmake_minimum_required(VERSION 3.26)
project(chainSim)

set(CMAKE_CXX_STANDARD 20)

add_executable(chainSim main.cpp
        ChainSim.cpp
        ChainSim.h
        purchase_policies/PurchasePolicy.h
        purchase_policies/PurchaseROP.cpp
        purchase_policies/PurchaseROP.h
        ChainSimBuilder.cpp
        ChainSimBuilder.h
        utils/ChainLogger.hpp
        utils/CLI.hpp
        third_party/argparse.hpp)
