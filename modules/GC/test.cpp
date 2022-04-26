#define GCDEBUG
#include "gc.hpp"
using namespace gc;
int main() 
{
    if (true) {
        Object<int> owo = 42;
        std::cout<< (int)owo << std::endl;
    }
    
    GC::collect();
    return 0;
}