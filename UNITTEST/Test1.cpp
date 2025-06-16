#include "src/QuoteBook.h"
#include "UNITTEST/Env.h"







TEST_F(MyTestSuite, TestOne) {

    spdlog::critical(" {}",a);

EXPECT_EQ(a, myquotebook_SERVER.NumLevels);
}

TEST_F(MyTestSuite, TestTwo) {
EXPECT_GT(2, 0);
}


TEST(SimpleStartUp,MyTest ) {
 //myquotebook_SERVER.printbook();

EXPECT_EQ(10, 10);


}
