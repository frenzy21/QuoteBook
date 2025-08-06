#include "spdlog/spdlog.h"
#include "src/QuoteBook.h"
#include <chrono>
#include <iostream>

#include <map>

int main() {
  std::cout << "Helloo, World!" << std::endl;

  std::cout << "Hello, World! James" << std::endl;
  spdlog::info("Welcome to spdlog!");
  spdlog::error("Some error message with arg: {}", 1);
  spdlog::warn("Easy padding in numbers like {:08d}", 12);
  spdlog::critical("Support for int: {0:d}; hex: {0:x}; oct: {0:o}; bin: {0:b}",
                   42);
  spdlog::info("Support for floats {:03.2f}", 1.23456);
  spdlog::info("Positional args are {1} {0}..", "too", "supported");
  int queuelength=100;
  QuoteBook myquotebook_SERVER("JPY", true, "Server",
                                         {"AA", "B", "C", "A", "D", "kk"}, queuelength,true);

  // QuoteBook<int,int> myquotebook_CLIENT("JPY",false,"client");
double startprice=myquotebook_SERVER.startPrice;
  // myquotebook_SERVER.printbook();
  myquotebook_SERVER.BookAddBid("AA", startprice, 30);
  myquotebook_SERVER.BookAddOffer("B", startprice, 1900);
  // myquotebook_CLIENT.printbook();
  // myquotebook_SERVER.printbook();
    spdlog::info("Starting Sleep");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    spdlog::info("Finished Sleep");
  myquotebook_SERVER.BookPrintOnUpdate();
  // spdlog::info("Position of src {} is {}..", "A",
  // myquotebook_CLIENT.getsrcindex("A"));
  //myquotebook_SERVER.bookPrintThread.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000*20));
    spdlog::info("Finished Sleep");
  return 0;
}
