//# include<gtest/gtest.h>
# include "spdlog/spdlog.h"
# include<gtest/gtest.h>
int Factorial(int n);

TEST(SimpleStartUp,MyTest ) {


EXPECT_EQ(Factorial(0), 10);


}



int Factorial(int n)
{
   return  n+1;
}