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
  double  startprice=myquotebook_CLIENT.startPrice;
    double  inc=myquotebook_CLIENT.myInc;

  int len=myquotebook_CLIENT.NumLevels;
  std::uniform_int_distribution<int> dist(0, len);
  int rn = dist(gen);
    spdlog::info(" Adding bid {} {} {} {} {} {}",rn,len,startprice,inc,startprice+inc*rn, 10 * rn);
  myquotebook_CLIENT.BookAddBid("C", startprice+inc*rn, 10 * rn);

    spdlog::info(" Added bid ");
  myquotebook_CLIENT.print();
  //myquotebook_CLIENT.unlockall();
  simulation(&myquotebook_CLIENT);
  //  myquotebook_CLIENT.unlockall();
}

void simulation(QuoteBook *myQuoteBook) {
  std::vector<std::string> Srcs = myQuoteBook->Srcs;
  std::uniform_int_distribution<int> dist(0, (myQuoteBook->NumLevels) - 1);
  std::uniform_int_distribution<int> distsrcs(0, Srcs.size() - 1);
    double  startprice=myQuoteBook->startPrice;
    double  inc=myQuoteBook->myInc;

    int len=myQuoteBook->NumLevels;
  auto start = std::chrono::high_resolution_clock::now();

  int random_number = dist(gen);
  // Perform operation
  int n=20000000;
    spdlog::info(" Starting Simulation.");
  for (int i = 0; i < n; ++i) {
      std::string s = Srcs[distsrcs(gen)];


    // spdlog::info(" after gen ");
      int rn = dist(gen);
    double p = startprice+inc*rn;
    int sz = (int)10 * rn;
     // spdlog::info(" Adding bid {} {} {} {} {} {}",rn,len,startprice,inc,p, sz);
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