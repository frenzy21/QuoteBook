#include <iostream>
#include "spdlog/spdlog.h"
#include "src/QuoteBook.h"

int main(int argc, char* argv[]) {

    int mykey=10;
    if (argc = 1) {
        spdlog::info(" Hello");
      mykey=  std::atoi(argv[1]);
    }



    QuoteBook<int,int> myquotebook_CLIENT("JPY",false);

    try {
        spdlog::info("Returning value for key {}= {}", mykey, myquotebook_CLIENT.MyMapGet(mykey));
    }catch (const std::exception& e) {
        spdlog::info("Exception caught: {}", e.what());
    }

    return 0;
}
