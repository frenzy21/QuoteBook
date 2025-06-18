#include "spdlog/spdlog.h"
#include "UNITTEST/BASIC/Env.h"
void MyBookBasic::SetUpTestCase() {
    std::cout << "Setting up test suite\n";
    static int call_count = 0;
    std::cout << "[Suite] SetUpTestSuite call #" << ++call_count << "\n";


    // Initialize once before any test runs
    spdlog::critical("Setup Test Suite..................................");
}

void MyBookBasic::TearDownTestCase() {
    // Optional cleanup
    std::cout << "Tear down test suite\n";
    spdlog::critical("TearDown Test Suite..................................");
}

void MyBookBasic::SetUp() {
    std::cout << "Setting\n";
    static int call_count = 0;



    // Initialize once before any test runs
    spdlog::critical("Setup..................................");
}

void MyBookBasic::TearDown() {
    // Optional cleanup
    std::cout << "Tear down\n";
    spdlog::critical("TearDown..................................");
}
