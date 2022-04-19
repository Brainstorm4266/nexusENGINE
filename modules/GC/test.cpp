#define GCDEBUG
#include "gc.hpp"
#include <iostream>
int main() 
{
    Object<int> o2 = Object<int>(5);
    std::cout<< o2.getrefCnt() << std::endl;
}