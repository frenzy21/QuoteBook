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

    spdlog::info(" Bid  level and price is {} {}",myenv.myquotebook_CLIENT->getBestBidLevel(),myenv.myquotebook_CLIENT->getBestBid());
    EXPECT_EQ(0,myenv.myquotebook_SERVER->getBestBidLevel())<<" Checking Bid Level is at Zero";
    EXPECT_NEAR(myenv.myquotebook_CLIENT->getBestBid(),sp,mi/10.)<<" Checking Bid Price is at start of rack";;
    EXPECT_EQ(myenv.myquotebook_CLIENT->getBestOfferLevel(),lvls)<<" Checking Offer Level is just above 1.0001";
    EXPECT_NEAR(myenv.myquotebook_CLIENT->getBestOffer(),sp+lvls*mi,mi/10.)<<" Checking Bid Price is at start of rack";;

    myenv.Replay(*myenv.myquotebook_SERVER,myreader.myData);
    //We now create a new bid and make it above the existing bid.
    myenv.myquotebook_CLIENT->BookAddBid( myenv.myquotebook_CLIENT->Srcs.at(1),1.00012,2100);
    EXPECT_EQ(myenv.myquotebook_CLIENT->getBestBidLevel(),12)<<"checking new bid level";
    EXPECT_NEAR(myenv.myquotebook_CLIENT->getBestBid(),1.00012,mi/10)<<"checking new bid price";
    spdlog::info("New  Offer  level and price is {} {}",myenv.myquotebook_CLIENT->getBestOfferLevel(),myenv.myquotebook_CLIENT->getBestOffer());


    //myenv.myquotebook_CLIENT->printbook();
}

