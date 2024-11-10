cmake_minimum_required(VERSION 3.15)
project(ChainSim)

set(CMAKE_CXX_STANDARD 20)

# Download and configure Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# Main library
add_library(chainsim_lib
    ChainSim.cpp
    ChainSimBuilder.cpp
    purchase_policies/PurchaseROP.cpp
    purchase_policies/PurchaseEOQ.cpp
    purchase_policies/PurchaseTPOP.cpp
)

# Main executable
add_executable(chainsim main.cpp)
target_link_libraries(chainsim PRIVATE chainsim_lib)

# Enable testing
enable_testing()

# Tests executable
add_executable(chainsim_tests
    tests/ChainSimTests.cpp
    tests/PurchasePolicyTests.cpp
    tests/ChainSimBuilderTests.cpp
)

target_link_libraries(chainsim_tests
    PRIVATE
    chainsim_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(chainsim_tests)
