
#include <iostream>
#include <sstream>
#include <string>

void foo(int x)
{
    for (int x = 0; x < 100; x++);

    std::cout << x << std::endl;
}

int main()
{
    foo(10);
}
