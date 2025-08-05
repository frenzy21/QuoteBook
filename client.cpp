#include "spdlog/spdlog.h"
#include "src/QuoteBook.h"
#include <iostream>
#include <random>

std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister PRNG

void simulation(QuoteBook *myQuoteBook);

int main(int argc, char *argv[]) {

  int mykey = 10;
  if (argc = 1) {
    spdlog::info(" Hello");
    mykey = std::atoi(argv[1]);
  }

  spdlog::info(" Hello Creating Object");
  QuoteBook myquotebook_CLIENT("JPY", false);
  spdlog::info(" Hello printing book");
  myquotebook_CLIENT.printbook();

  std::uniform_int_distribution<int> dist(0, 4);
  int random_number = dist(gen);
  myquotebook_CLIENT.BookAddOffer("C", dist(gen), 10 * dist(gen));
  myquotebook_CLIENT.print();
  //myquotebook_CLIENT.unlockall();
  simulation(&myquotebook_CLIENT);
  //  myquotebook_CLIENT.unlockall();
}

void simulation(QuoteBook *myQuoteBook) {
  std::vector<std::string> Srcs = myQuoteBook->Srcs;
  std::uniform_int_distribution<int> dist(0, (myQuoteBook->NumLevels) - 1);
  std::uniform_int_distribution<int> distsrcs(0, Srcs.size() - 1);

  auto start = std::chrono::high_resolution_clock::now();

  int random_number = dist(gen);
  // Perform operation
  int n=20000;
    spdlog::info(" Starting Simulation.");
  for (int i = 0; i < n; ++i) {
      std::string s = Srcs[distsrcs(gen)];


    // spdlog::info(" after gen ");
    int p = dist(gen);
    int sz = (int)10 * dist(gen);
    // spdlog::info(" {} {} {}0",s,p,sz);
    myQuoteBook->BookAddBid(s, p, sz);
  }
  // End time
  auto stop = std::chrono::high_resolution_clock::now();

  // Calculate duration
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

  spdlog::info("Time taken: {}", duration.count());

  spdlog::info("Average number of updates per second is  {}",1000000*((float)n)/(float) duration.count());
}