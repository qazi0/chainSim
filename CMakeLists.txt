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
        purchase_policies/PurchaseTPOP.cpp
        purchase_policies/PurchaseTPOP.h
        purchase_policies/PurchaseEOQ.cpp
        purchase_policies/PurchaseEOQ.h)
