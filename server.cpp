#include <iostream>
#include "spdlog/spdlog.h"
#include "src/QuoteBook.h"
#include <chrono>

#include <map>

int main() {
    std::cout << "Helloo, World!" << std::endl;

    std::cout << "Hello, World! James" << std::endl;
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d}; hex: {0:x}; oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    QuoteBook<int,int> myquotebook_SERVER("JPY",true);
    myquotebook_SERVER.print();
    QuoteBook<int,int> myquotebook_CLIENT("JPY",false);
    myquotebook_CLIENT.print();

    myquotebook_SERVER.MyMapSet(1,-10);

    spdlog::info("Returning value for key 1= {}",myquotebook_CLIENT.MyMapGet(1));


        auto start = std::chrono::high_resolution_clock::now();

        // Perform operation
    for (int i = 0; i < 200000; ++i)
    {
        myquotebook_SERVER.MyMapSet(i%1000,i);
       // spdlog::info( "{} {} {}", i%1000,i,myquotebook_SERVER.MyMapGet(i%1000));
    }
        // End time
        auto stop = std::chrono::high_resolution_clock::now();

        // Calculate duration
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        spdlog::info( "Time taken: {}", duration.count());

    spdlog::info("Returning value for key 100= {}",myquotebook_SERVER.MyMapGet(100));
    return 0;
}
