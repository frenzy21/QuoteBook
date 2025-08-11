#include "spdlog/spdlog.h"
#include "UNITTEST/BOOKUPDATE/Env.h"
#include "UNITTEST/src/MyBookReader.h"
void MyBookUpdate::SetUpTestCase() {
    std::cout << "Setting up test suite\n";
    static int call_count = 0;
    MyBookReader myreader=MyBookReader();
    myenv.myquotebook_SERVER->clearBook();
    // Initialize once before any test runs
    spdlog::info("Setup Test Suite..................................");
}

void MyBookUpdate::TearDownTestCase() {
    // Optional cleanup
    std::cout << "Tear down test suite\n";
    spdlog::critical("TearDown Test Suite..................................");
}

void MyBookUpdate::SetUp() {
    std::cout << "Setting\n";
    static int call_count = 0;



    // Initialize once before any test runs
    spdlog::critical("Setup..................................");
}

void MyBookUpdate::TearDown() {
    // Optional cleanup
    std::cout << "Tear down\n";
    spdlog::critical("TearDown..................................");
}

MyEnv  MyBookUpdate::myenv;
