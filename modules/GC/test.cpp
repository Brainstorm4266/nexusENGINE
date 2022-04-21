#define GCDEBUG
#include "gc.hpp"
#include <iostream>
int main() 
{
    using namespace gc_old;
    Object<Object<int>> o2 = Object<int>(52);
    std::cout<< o2.getrefCnt() << std::endl;
    Object<Object<int>> o1 = o2;
    std::cout<< o2.getrefCnt() << std::endl;
    
}