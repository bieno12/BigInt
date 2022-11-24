#include <iostream>
#include "BigInt.h"

using namespace std;

int main()
{
    
    BigInt num1(0,{});

    BigInt num2(4541);
    
    BigInt num3(32);
    BigInt result = num2 / num3;

    cout << result.str(1) << endl;
    cout << num2 << " / " << num3 << " = " << result << endl;
    cout << num2 << " % " << num3 << " = " << num2 % num3 << endl;

}