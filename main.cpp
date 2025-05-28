#include <iostream>
#include "spdlog/spdlog.h"
#include "src/QuoteBook.h"

int main() {
    std::cout << "Helloo, World!" << std::endl;

    std::cout << "Hello, World! James" << std::endl;
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d}; hex: {0:x}; oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    QuoteBook<int,int> myquotebook_int(10);
    myquotebook_int.print();
    QuoteBook<float,float> myquotebook_float(10.);
    myquotebook_float.print();
    return 0;
}
