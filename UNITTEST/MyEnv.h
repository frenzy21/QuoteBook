#include "src/QuoteBook.h"
#include "spdlog/spdlog.h"
class MyEnv{
public:
    int a=1;
    int sharedValue=0;
    MyEnv()
    : myquotebook_SERVER("JPY", true, "Server",{"AA", "B", "C", "A", "D", "kk"}, 20,true),
     myquotebook_CLIENT("JPY", false,"Client")
    {
        spdlog::info("Creating an Environment to test against");
    }

    ~MyEnv(){}
    public:
        QuoteBook<int, int> myquotebook_SERVER;
        QuoteBook<int, int> myquotebook_CLIENT;

};
