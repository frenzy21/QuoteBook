#include <iostream>
#include "spdlog/spdlog.h"
#include "src/QuoteBook.h"

int main(int argc, char* argv[]) {

    int mykey=10;
    if (argc = 1) {
        spdlog::info(" Hello");
      mykey=  std::atoi(argv[1]);
    }


    spdlog::info(" Hello Creating Object");
    QuoteBook<int,int> myquotebook_CLIENT("JPY",false);
    spdlog::info(" Hello printing book");
    myquotebook_CLIENT.printbook();
    //myquotebook_CLIENT.BooKAdd("C",0,900);


    /*auto start = std::chrono::high_resolution_clock::now();

    // Perform operation
    for (int i = 0; i < 2000000; ++i)
    {
       //int y= myquotebook_CLIENT.MyMapGet(i%1000);
        // spdlog::info( "{} {} {}", i%1000,i,myquotebook_SERVER.MyMapGet(i%1000));
    }
    // End time
    auto stop = std::chrono::high_resolution_clock::now();

    // Calculate duration
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    spdlog::info( "Time taken: {}", duration.count());
    return 0;
     */
}