#include<gtest/gtest.h>
#include "src/QuoteBook.h"
#include "UNITTEST/MyEnv.h"

class MyTestSuite : public ::testing::Test {
protected:
    static void SetUpTestCase();  // Declaration
    static void TearDownTestCase();
    void SetUp();  // Declaration
    void TearDown();

public:
    MyEnv myenv;
};






