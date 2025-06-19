#include "src/QuoteBook.h"
#include "UNITTEST/BOOKUPDATE/Env.h"
#include <boost/unordered_map.hpp>
#include <map>



//Basic tests to check that shared memory created ok.



//Check that MyEnv has the same number of Cols and rows.
TEST_F(MyBookUpdate, AddPrice) {


}


//This test Will Check the sources Match.
TEST_F(MyBookUpdate, RemovePrice) {


}

//Checks teh Pid map Was made correctly
TEST_F(MyBookUpdate, AddSrcThatDoesntExist) {

}

//Checks the printbook member operates correctly
TEST_F(MyBookUpdate, GetXLevel) {

EXPECT_EQ(0,myenv.myquotebook_SERVER.

getlevel(1))<<" Expected Pid maps to Match";

}

TEST_F(MyBookUpdate, GetLevelThatDoesntExist) {

ASSERT_NO_FATAL_FAILURE(myenv.myquotebook_SERVER.printbook());
ASSERT_NO_FATAL_FAILURE(myenv.myquotebook_CLIENT.printbook());

}

