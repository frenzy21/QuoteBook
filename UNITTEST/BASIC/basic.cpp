#include "src/QuoteBook.h"
#include "UNITTEST/BASIC/Env.h"
#include <boost/unordered_map.hpp>
#include <map>



//Basic tests to check that shared memory created ok.



//Check that MyEnv has the same number of Cols and rows.
TEST_F(MyBookBasic, NumLevelsAndRows) {

int S=myenv.myquotebook_SERVER.NumLevels;
int C=myenv.myquotebook_CLIENT.NumLevels;

int S_ROWS=myenv.myquotebook_SERVER.myState->rows;
int S_COLS=myenv.myquotebook_CLIENT.myState->cols;

int C_ROWS=myenv.myquotebook_SERVER.myState->rows;
int C_COLS=myenv.myquotebook_CLIENT.myState->cols;

EXPECT_EQ(S,C)<< "Expected NumLevels to Match.";

EXPECT_EQ(S_ROWS,C_ROWS)<< "Expected Rows to Match.";
EXPECT_EQ(S_COLS,C_COLS)<< "Expected Cols to Match.";

}


//This test Will Check the sources Match.
TEST_F(MyBookBasic, Srcs) {
    std::vector<std::string> S_SRCS=myenv.myquotebook_SERVER.Srcs;
    std::vector<std::string> C_SRCS=myenv.myquotebook_CLIENT.Srcs;
    EXPECT_EQ(S_SRCS.size(),C_SRCS.size())<<" Expected Src Sizes To Match";
    EXPECT_EQ(S_SRCS,C_SRCS)<<" Expected Elements to Match";

}

//Checks teh Pid map Was made correctly
TEST_F(MyBookBasic, PidMap) {
    //This test is a little annoying because teh PIDs in the unit tests match. How to solve this?
  std::map<int, std::string> S_PIDMAP(myenv.myquotebook_SERVER.myState->myPidMap->begin(),myenv.myquotebook_SERVER.myState->myPidMap->end());
  std::map<int, std::string> C_PIDMAP(myenv.myquotebook_CLIENT.myState->myPidMap->begin(),myenv.myquotebook_CLIENT.myState->myPidMap->end());
  //C_PIDMAP.erase(std::prev(C_PIDMAP.end()));
  EXPECT_EQ(S_PIDMAP,C_PIDMAP)<<" Expected Pid maps to Match";
}

//Checks the printbook member operates correctly
//TEST_F(MyBookBasic, PrintBook) {

//ASSERT_NO_FATAL_FAILURE(myenv.myquotebook_SERVER.printbook());
//ASSERT_NO_FATAL_FAILURE(myenv.myquotebook_CLIENT.printbook());

//}
