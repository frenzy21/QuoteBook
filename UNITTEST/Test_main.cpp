#include "UNITTEST/Env.h"
#include<gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>










int main(int argc, char **argv) {


    auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/output.log");
    spdlog::set_default_logger(file_logger);



    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}