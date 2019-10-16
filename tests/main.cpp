#include "test.h"


Test(FirstTest)
{
    int a = 5;
    int b = 3;

    Equals(a, b);
    Equals(a, 5);
}


int main()
{
    RunAllTests();
    RunTest(FirstTest);
}