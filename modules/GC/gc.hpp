#include <vector>
#include <exception>
#include <iostream>
#include <algorithm>

#pragma once
namespace gc_old {
    using std::vector;
    template <typename T>
    class _GCHead 
    {
    public:
        const volatile size_t size = sizeof(T);
        long long refcnt = 0;
        long long inactiverefcnt = 0;
        T data = {0};
        long long activeRefCnt() 
        {
            return refcnt-inactiverefcnt;
        }
    };
    class GenericObject;
    template <typename T>
    class Object 
    {
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
        operator T() {
            return data->data;
        }
        operator T*() {
            return data->data;
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
        template <typename T>
        GenericObject(const Object<T>& o) {
            data = (_GCHead<void*>*)o.data;
            data->refcnt++;
            #ifdef GCDEBUG
            std::cout << "GC generic object copy" << std::endl;
            #endif
        }
        GenericObject(const GenericObject& o) {
            data = o.data;
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
        
        template <typename T>
        operator T () {
            if (sizeof(T) != data->size) throw std::bad_cast("Size of T is not equal to size of data");
            return *(data->data);
        }
        template <typename T>
        operator T*() {
            if (sizeof(T) != data->size) throw std::bad_cast("Size of T is not equal to size of data");
            return data->data;
        }
        template <typename T>
        operator Object<T>();
    };
}
template <typename T>
gc_old::GenericObject::operator gc_old::Object<T>() {
    if(sizeof(T) != data->size) throw std::bad_cast("Size of T is not equal to size of data");
    return gc_old::Object<T>(*(data->data));
}
template <typename T>
gc_old::Object<T>::operator gc_old::GenericObject() {
    return gc_old::GenericObject(*(data->data));
}
namespace gc {
    using std::vector;
    template <typename T>
    class Object;
    class Generic;
    class GC;
    template <typename T>
    class GCObject;
    class GCObjectHead
    {
    public:
        vector<GCObjectHead*> object_to_ref;
        vector<GCObjectHead*> ref_to_object;
        unsigned long long Crefs = 0;
        const volatile size_t size = 0;
        volatile size_t arrlen = 1;
        size_t getTotalSize();
        size_t getTotalRefs();
    };
    size_t GCObjectHead::getTotalSize() {
        return size*arrlen;
    }
    size_t GCObjectHead::getTotalRefs() {
        return object_to_ref.size()+Crefs;
    }
    template <typename T>
    class GCObject : public GCObjectHead
    {
    public:
        T data;
        const volatile size_t size = sizeof(T);
    };
    class GC final {
        GC() {}
        friend class GCObjectHead;
        template <typename T>
        friend class Object;
        friend class Generic;
    protected:
        vector<GCObjectHead*> objects;
    public:
        //make this a singleton
        static GC& getInstance() {
            static GC instance;
            return instance;
        }
        GC(const GC&) = delete;
        GC& operator=(const GC&) = delete;


    };
    template <typename T>
    class Object {
    protected:
        GCObject<T>* data = nullptr;
        long long memcheck() {
            GCObjectHead* b = (GCObjectHead*)(GC::getInstance().objects.begin());
            GCObjectHead* e = (GCObjectHead*)(GC::getInstance().objects.end());
            if (this > b && this < e) {
                return (this-b) % sizeof(T);
            }
            return -1;
        }
        void do_mem() {
            long long l = memcheck();
            if (l != -1) {
                #ifdef GCDEBUG
                std::cout << "Found references to objects" << std::endl;
                #endif
                //is in vector
                if (std::find(data->ref_to_object.begin(), data->ref_to_object.end(), GC::objects[l])) {
                    #ifdef GCDEBUG
                    std::cout << "Already processed reference" << std::endl;
                    #endif
                    return;
                }
                data->ref_to_object.push_back(GC::objects[l]);
                GC::objects[l]->object_to_ref.push_back(data);
            }
        }
    public:
        Object() : data(nullptr) {}
        Object(const Object& o) {
            #ifdef GCDEBUG
            std::cout<< "GC object copy" << std::endl;
            #endif
            data = o.data;
            data->Crefs++;
            #ifdef GCDEBUG
            std::cout << "GC new object increment ref" << std::endl;
            #endif
            do_mem();
        }
    };
}