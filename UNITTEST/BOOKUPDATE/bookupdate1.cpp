#include "src/QuoteBook.h"
#include "UNITTEST/BOOKUPDATE/Env.h"
#include <boost/unordered_map.hpp>
#include <map>




//Basic tests to check that levels in main Bid and Offer vectors update correctly.





//Check that MyEnv has the same number of Cols and rows.
TEST_F(MyBookUpdate, AddPrice) {
    int size=0;
    myenv.myquotebook_SERVER.BookAddBid("AA", 2, 30);
     myenv.myquotebook_SERVER.BookAddOffer("B", 4, 1900);

     size=myenv.myquotebook_CLIENT.getSizeBids(2);
     EXPECT_EQ(30,size)<<"Expected Bid size just added to match.";
size=myenv.myquotebook_CLIENT.getSizeOffer(4);
EXPECT_EQ(1900,size)<<"Expected Offer size just added to match.";

myenv.myquotebook_SERVER.BookAddBid("B", 2, 30);
myenv.myquotebook_SERVER.BookAddOffer("AA", 4, 1900);

size=myenv.myquotebook_CLIENT.getSizeBids(2);
EXPECT_EQ(2*30,size)<<"Expected Bid size just added to match when including a second source.";
size=myenv.myquotebook_CLIENT.getSizeOffer(4);
EXPECT_EQ(2*1900,size)<<"Expected Offer size just added to match when including a second source.";

size=myenv.myquotebook_SERVER.getSizeBids(2);
EXPECT_EQ(2*30,size)<<"Expected Bid size just added to match when including a second source.";
size=myenv.myquotebook_SERVER.getSizeOffer(4);
EXPECT_EQ(2*1900,size)<<"Expected Offer size just added to match when including a second source.";


size=myenv.myquotebook_CLIENT.getSizeBids(2);
EXPECT_EQ(0,size)<<"Expected Bid size return to zero when clearing.";
size=myenv.myquotebook_CLIENT.getSizeOffer(4);
EXPECT_EQ(0,size)<<"Expected Offer size return to zero when clearing.";

}



//Checks the Pid map Was made correctly
TEST_F(MyBookUpdate, AddSrcThatDoesntExist) {

}

//Checks the printbook member operates correctly
TEST_F(MyBookUpdate, GetXLevel) {

  //EXPECT_EQ(0,myenv.myquotebook_SERVER.

 //getlevel(1))<<" Expected Pid maps to Match";

}

TEST_F(MyBookUpdate, GetLevelThatDoesntExist) {

ASSERT_NO_FATAL_FAILURE(myenv.myquotebook_SERVER.printbook());
ASSERT_NO_FATAL_FAILURE(myenv.myquotebook_CLIENT.printbook());

}

