#include <iostream>
#include "spdlog/spdlog.h"
#include "src/QuoteBook.h"

std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister PRNG


void simulation(QuoteBook<int, int> *myQuoteBook);

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
    std::uniform_int_distribution<int> dist(0, 4);
    int random_number = dist(gen);
    myquotebook_CLIENT.BooKAdd("C",dist(gen),10*dist(gen));


    simulation(&myquotebook_CLIENT);

}

void simulation(QuoteBook<int, int> *myQuoteBook) {
    std::vector<std::string> Srcs= myQuoteBook->Srcs;
    std::uniform_int_distribution<int> dist(0, (myQuoteBook->NumLevels)-1);
    std::uniform_int_distribution<int> distsrcs(0, Srcs.size()-1);

    auto start = std::chrono::high_resolution_clock::now();

    spdlog::info(" Starting Test");
int random_number = dist(gen);
    // Perform operation
    for (int i = 0; i < 200000000; ++i)
    {
       // spdlog::info(" start of loop ");
        std::string s=Srcs[distsrcs(gen)];
       // spdlog::info(" after gen ");
        int p=dist(gen);
        int sz=(int)10*dist(gen);
       // spdlog::info(" {} {} {}0",s,p,sz);
        myQuoteBook->BooKAdd(s,p,sz);

    }
    // End time
    auto stop = std::chrono::high_resolution_clock::now();

    // Calculate duration
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    spdlog::info( "Time taken: {}", duration.count());


}