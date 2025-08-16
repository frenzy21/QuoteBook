#include "src/QuoteBook.h"
#include "UNITTEST/BOOKUPDATE/Env.h"
#include <boost/unordered_map.hpp>
#include <map>




//Basic tests to check that levels in main Bid and Offer vectors update correctly.





//First book add unit test.
TEST_F(MyBookUpdate, AddRecords) {
    EXPECT_EQ(0,0)<<" Test test";
    myreader.readpath("UNITTEST/DATA/book1.csv");
    EXPECT_GT(myreader.myData.size(),0)<<"Expected data read in to have multiple rows.";
    double sp=myenv.myquotebook_SERVER->startPrice;
    double mi=myenv.myquotebook_SERVER->myInc;
    int lvls=myenv.myquotebook_SERVER->NumLevels;
    //myenv.Replay(*myenv.myquotebook_SERVER,myreader.myData);
    spdlog::info(" Bid  level and price is {} {}",myenv.myquotebook_CLIENT->getBestBidLevel(),myenv.myquotebook_CLIENT->getBestBid());
    EXPECT_EQ(0,myenv.myquotebook_SERVER->getBestBidLevel())<<" Checking Bid Level is at Zero";
    EXPECT_EQ(myenv.myquotebook_CLIENT->getBestBid(),sp)<<" Checking Bid Price is at start of rack";;
    EXPECT_EQ(myenv.myquotebook_CLIENT->getBestOfferLevel(),lvls)<<" Checking Offer Level is just above 1.0001";
    //We now create a new bid and make it above the existing bid.
    //myenv.myquotebook_CLIENT->BookAddBid( myenv.myquotebook_CLIENT->Srcs.at(0),1.0002,2000);
    //spdlog::info("New  Bid  level and price is {} {}",myenv.myquotebook_CLIENT->getBestBidLevel(),myenv.myquotebook_CLIENT->getBestBid());
    //myenv.myquotebook_CLIENT->printbook();
}

