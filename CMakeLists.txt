cmake_minimum_required(VERSION 3.26)
project(chainSim)

set(CMAKE_CXX_STANDARD 20)

add_executable(chainSim main.cpp
        ChainSim.cpp
        ChainSim.h
        purchase_methods/PurchaseMethod.h
        purchase_methods/PurchaseROP.cpp
        purchase_methods/PurchaseROP.h
        ChainSimBuilder.cpp
        ChainSimBuilder.h
        utils/ChainLogger.hpp)
