#include "src/QuoteBook.h"
#include "UNITTEST/BOOKUPDATE/Env.h"
#include <boost/unordered_map.hpp>
#include <map>




//Basic tests to check that levels in main Bid and Offer vectors update correctly.





//Check that MyEnv has the same number of Cols and rows.
TEST_F(MyBookUpdate, AddRecords) {
    EXPECT_EQ(0,0)<<" Test test";
    myreader.readpath("UNITTEST/DATA/book1.csv");
    EXPECT_GT(myreader.myData.size(),0)<<"Expected data read in to have multiple rows.";
    myenv.Replay(myenv.myquotebook_SERVER,myreader.myData);
    spdlog::info(" Bid and Offer at level 1 is {}  {}",myenv.myquotebook_CLIENT.GetLevelBid(1),myenv.myquotebook_CLIENT.GetLevelBid(1));

}

