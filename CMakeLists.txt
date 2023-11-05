cmake_minimum_required(VERSION 3.26)
project(chainSim)

set(CMAKE_CXX_STANDARD 20)

add_executable(chainSim main.cpp
        ChainSim.cpp
        ChainSim.h
        PurchaseMethod.h
        PurchaseROP.cpp
        PurchaseROP.h
        ChainSimBuilder.cpp
        ChainSimBuilder.h)
