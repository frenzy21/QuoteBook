# include<gtest/gtest.h>
# include "spdlog/spdlog.h"
#include "src/QuoteBook.h"
int Factorial(int n);

QuoteBook<int, int> myquotebook_SERVER("JPY", true, "Server",{"AA", "B", "C", "A", "D", "kk"}, 20,true);

QuoteBook<int, int> myquotebook_CLIENT("JPY", true, "CLIENT");

// QuoteBook<int,int> myquotebook_CLIENT("JPY",false,"client");

// QuoteBook<int,int> myquotebook_CLIENT("JPY",false,"client");

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}