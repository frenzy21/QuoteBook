#include "spdlog/spdlog.h"
#include "src/QuoteBook.h"
#include <iostream>
#include <random>
#include <thread>
#include <chrono>



std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister PRNG


int main(int argc, char *argv[]) {

  int mykey = 10;
  if (argc == 1) {
    spdlog::info(" Hello");
    mykey = std::atoi(argv[1]);
  }

  spdlog::info(" Hello Creating Object");
  QuoteBook myquotebook_CLIENT("JPY", false);
  spdlog::info(" Hello printing book");
  myquotebook_CLIENT.printbook();
  int i=0;

  while(i<=10)
  {
      spdlog::info(" ##################################");
      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      myquotebook_CLIENT.printbook();
      spdlog::info(" ##################################");

  }
}

