cmake_minimum_required(VERSION 3.16)

project(ChainSimQServe LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS 
    Core
    Network
    HttpServer
)

add_executable(ChainSimQServe
  main.cpp
  ChainSimBuilder.h ChainSimBuilder.cpp
  ChainSim.h ChainSim.cpp
  ChainSimServer.h ChainSimServer.cpp
  purchase_policies/PurchasePolicy.h
  purchase_policies/PurchaseROP.h purchase_policies/PurchaseROP.cpp
  purchase_policies/PurchaseTPOP.h purchase_policies/PurchaseTPOP.cpp
  purchase_policies/PurchaseEOQ.h purchase_policies/PurchaseEOQ.cpp
  utils/ChainLogger.hpp
  utils/CLI.hpp
  utils/DemandSampler.hpp
)

target_link_libraries(ChainSimQServe PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::HttpServer
)

include(GNUInstallDirs)
install(TARGETS ChainSimQServe
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
