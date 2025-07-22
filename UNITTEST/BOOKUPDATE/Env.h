#include<gtest/gtest.h>
#include "src/QuoteBook.h"
#include "UNITTEST/MyEnv.h"
#include "UNITTEST/src/MyBookReader.h"

class MyBookUpdate: public ::testing::Test{
public:  explicit MyBookUpdate():myreader("UNITTEST/DATA/book1.csv")
    {}
protected:
    static void SetUpTestCase();  // Declaration
    static void TearDownTestCase();
    void SetUp();  // Declaration
    void TearDown();

public:
    MyEnv myenv;
    MyBookReader myreader;
};






