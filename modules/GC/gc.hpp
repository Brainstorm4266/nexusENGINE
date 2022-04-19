#include <vector>
#include <exception>
#include <iostream>

#pragma once

using std::vector;
template <typename T>
class _GCHead {
public:
    const volatile size_t size = sizeof(T);
    long long refcnt = 0;
    T data = {0};
};
class GenericObject;
template <typename T>
class Object {
protected:
    _GCHead<T>* data = nullptr;
public:
    Object() : data(nullptr) {}
    Object(const Object& o) {
        #ifdef GCDEBUG
        std::cout<< "GC object copy" << std::endl;
        #endif
        data = o.data;
        data->refcnt++;
        #ifdef GCDEBUG
        std::cout << "GC new object increment ref" << std::endl;
        #endif
        
    }
    Object(T o) {
        #ifdef GCDEBUG
        std::cout << "GC new object" << std::endl;
        #endif
        data = new _GCHead<T>;
        data->data = o;
        data->refcnt = 1;
    }
    T* operator->() {
        return data->data;
    }
    T& operator*() {
        return data->data;
    }
    Object& operator=(const Object& o) {
        #ifdef GCDEBUG
        std::cout << "GC object copy" << std::endl;
        #endif
        if (data != nullptr) {
            data->refcnt--;
            #ifdef GCDEBUG
            std::cout << "GC old object decrement ref" << std::endl;
            #endif
            if (data->refcnt == 0) {
                std::cout << "GC delete old object" << std::endl;
                delete data;
            }
        }
        data = o.data;
        data->refcnt++;
        #ifdef GCDEBUG
        std::cout << "GC new object increment ref" << std::endl;
        #endif
        return *this;
    }
    operator T() {
        return data->data;
    }
    Object& operator=(const T& o) {
        if (data != nullptr) {
            data->refcnt--;
            #ifdef GCDEBUG
            std::cout << "GC old object decrement ref" << std::endl;
            #endif
            if (data->refcnt == 0) {
                #ifdef GCDEBUG
                std::cout << "GC delete old object" << std::endl;
                #endif
                delete data;
            }
        }
        data->data = o;
        data->refcnt = 1;
        #ifdef GCDEBUG
        std::cout << "GC created new object" << std::endl;
        #endif
        return *this;
    }
    ~Object() {
        #ifdef GCDEBUG
        std::cout << "GC object destructor" << std::endl;
        #endif
        if (data != nullptr) {
            data->refcnt--;
            #ifdef GCDEBUG
            std::cout << "GC old object decrement ref" << std::endl;
            #endif
            if (data->refcnt == 0) {
                delete data;
                #ifdef GCDEBUG
                std::cout << "GC delete old object" << std::endl;
                #endif
            }
        }
    }
    operator GenericObject();
    long long getrefCnt() {
        return data->refcnt;
    }
};
class GenericObject : protected Object<void*>
{
public:
    GenericObject() : Object<void*>() {}
    template <typename T>
    GenericObject(T v) : Object<void*>()
    {
        #ifdef GCDEBUG
        std::cout << "GC new generic object" << std::endl;
        #endif
        data = (_GCHead<void*>*)(new _GCHead<T>);
        ((_GCHead<T>*)data)->data = v;
        data->refcnt = 1;
    }
    void* operator*() {
        return data->data;
    }
    template<typename T>
    T* operator*() {
        if (sizeof(T) != data->size) throw std::bad_cast("Size of T is not equal to size of data");
        return data->data;
    }
    template<typename T>
    T* operator->() {
        if (sizeof(T) != data->size) throw std::bad_cast("Size of T is not equal to size of data");
        return data->data;
    }
    GenericObject& operator=(const GenericObject& o) {
        #ifdef GCDEBUG
        std::cout << "GC generic object copy" << std::endl;
        #endif
        if (data != nullptr) {
            data->refcnt--;
            #ifdef GCDEBUG
            std::cout << "GC old generic object decrement ref" << std::endl;
            #endif
            if (data->refcnt == 0) {
                delete data;
                #ifdef GCDEBUG
                std::cout << "GC delete old generic object" << std::endl;
                #endif
            }
        }
        data = o.data;
        data->refcnt++;
        #ifdef GCDEBUG
        std::cout << "GC new generic object increment ref" << std::endl;
        #endif
        return *this;
    }
    template <typename T>
    GenericObject& operator=(const Object<T>& o) {
        #ifdef GCDEBUG
        std::cout << "GC generic object copy" << std::endl;
        #endif
        if (data != nullptr) {
            data->refcnt--;
            #ifdef GCDEBUG
            std::cout << "GC old generic object decrement ref" << std::endl;
            #endif
            if (data->refcnt == 0) {
                delete data;
                #ifdef GCDEBUG
                std::cout << "GC delete old generic object" << std::endl;
                #endif
            }
        }
        data = (_GCHead<void*>*)o.data;
        data->refcnt++;
        #ifdef GCDEBUG
        std::cout << "GC new generic object increment ref" << std::endl;
        #endif
        return *this;
    }
    template <typename T>
    GenericObject& operator=(const T& o) {
        if (data != nullptr) {
            data->refcnt--;
            #ifdef GCDEBUG
            std::cout << "GC old generic object decrement ref" << std::endl;
            #endif
            if (data->refcnt == 0) {
                delete data;
                #ifdef GCDEBUG
                std::cout << "GC delete old generic object" << std::endl;
                #endif
            }
        }
        data = (_GCHead<void*>*)(new _GCHead<T>);
        ((_GCHead<T>*)data)->data = o;
        data->refcnt = 1;
        #ifdef GCDEBUG
        std::cout << "GC new generic object" << std::endl;
        #endif
        return *this;
    }
    template <typename T>
    GenericObject(const Object<T>& o) {
        data = (_GCHead<void*>*)o.data;
        data->refcnt++;
        #ifdef GCDEBUG
        std::cout << "GC generic object copy" << std::endl;
        #endif
    }
    ~GenericObject() {
        #ifdef GCDEBUG
        std::cout << "GC generic object destructor" << std::endl;
        #endif
        if (data != nullptr) {
            data->refcnt--;
            #ifdef GCDEBUG
            std::cout << "GC old generic object decrement ref" << std::endl;
            #endif
            if (data->refcnt == 0) {
                delete data;
                #ifdef GCDEBUG
                std::cout << "GC delete old generic object" << std::endl;
                #endif
            }
        }
    }
    GenericObject(const GenericObject& o) {
        data = o.data;
        data->refcnt++;
        #ifdef GCDEBUG
        std::cout << "GC generic object copy" << std::endl;
        #endif
    }
    template <typename T>
    operator T () {
        if (sizeof(T) != data->size) throw std::bad_cast("Size of T is not equal to size of data");
        return *data->data;
    }
    template <typename T>
    operator Object<T>();
};