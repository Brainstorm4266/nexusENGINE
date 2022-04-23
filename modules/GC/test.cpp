#define GCDEBUG
#include "gc.hpp"
#include <iostream>
using namespace gc;
class LinkedList {
public:
    Object<LinkedList> next;
    Object<LinkedList> prev;
    LinkedList();
    static LinkedList new_next(LinkedList next) {
        LinkedList l;
        l.next = next;
        return l;
    }
    static LinkedList new_prev(LinkedList prev) {
        LinkedList l;
        l.prev = prev;
        return l;
    }
    LinkedList(LinkedList next, LinkedList prev)
        : next(next), prev(prev) {};
    LinkedList& end() {
        LinkedList& v = *this;
        while ((LinkedList*)(v.next) != nullptr) {
            v = *(v.next);
        }
        return v;
    }
    LinkedList& begin() {
        LinkedList& v = *this;
        while ((LinkedList*)(v.prev) != nullptr) {
            v = *(v.prev);
        }
        return v;
    }
    void add_end() {
        LinkedList& v = end();
        v.next = new_prev(v);
    }
    void add_begin() {
        LinkedList& v = begin();
        v.prev = new_next(v);
    }
};
class O {
public:
    Object<O> o;
    O() : o(nullptr) {};
    
};
int main() 
{
    if (true) {
        O o;
        o.o = O();
        o.o->o = o;
    }
    
    GC::collect();
    return 0;
}