#include <vector>
#include <exception>
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <unordered_map>
#include <cstdlib>

#include "../LIB/Events.hpp"

#pragma once
#ifdef GCDEBUG
#define GC_PRINTOBJ_REF(i) std::cout << "GC object with refcnt " << i->Crefs << " and active " << i->getActiveRefs() << std::endl;
#else
#define GC_PRINTOBJ_REF(i)
#endif
unsigned long long roundinchunks(unsigned long long num, unsigned long long chunksize) {
    if ((num % chunksize) == 0) return num;
    return (chunksize - (num % chunksize) + num);
}
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
    using std::unordered_map;
    template <typename T>
    class Object;
    class Generic;
    class GC;
    template <typename T>
    class GCObject;
    enum RefType {
        STRONG,
        SELF,
        WEAK
    };  
    class GCObjectHead
    {
    private:
        EventCaller<> e;
    public:
        friend class GC;
        friend class GCAllocator;
        vector<GCObjectHead*> object_to_ref;
        vector<GCObjectHead*> ref_to_object;
        //ref types
        // strong when object to ref 
        // self when both
        unsigned long long Crefs = 0; //strong ref
        volatile size_t size = 0;
        volatile size_t arrlen = 1;
        size_t getTotalSize();
        size_t getTotalRefs();
        size_t getActiveRefs();
        Event<> onDestroy;
        GCObjectHead() :
            e(onDestroy())
        {}
        size_t getSelfRefs();
    };
    class GCIterator {
        friend class GC;
        friend class GCAllocator;
        GCObjectHead* head;
        GCAllocator* obj;
        size_t index;
        size_t end;
        GCIterator(size_t head, size_t end, GCAllocator* obj) :
            head((GCObjectHead*)head),
            index(0),
            end(end),
            obj(obj)
        {}
        GCObjectHead& operator*() {
            jumpiferased();
            return *((GCObjectHead*)((size_t)head + index));
        }
        GCObjectHead* operator->() {
            jumpiferased();
            return (GCObjectHead*)((size_t)head + index);
        }
        operator GCObjectHead*() {
            jumpiferased();
            return (GCObjectHead*)((size_t)head + index);
        }
        GCIterator& operator++(int);
        bool isAtEnd() {
            return this->operator*().getTotalSize() + index + sizeof(GCObjectHead) == end;
        }
        void jumpiferased();
    };
    class GCAllocator {
        friend class GC;
        friend class GCIterator;
        size_t capacity = 0;
        void* data = nullptr;
        unordered_map<size_t, size_t> erased;
        size_t allocatenew(size_t size) {
            size_t newcap = size + sizeof(GCObjectHead);
            if (newcap > (capacity)) {
                capacity += newcap;
                if (data == nullptr) data = malloc(capacity);
                else data = realloc(data, capacity);
            }
            (*((GCObjectHead*)(capacity-newcap + (size_t)data))) = GCObjectHead();
            return capacity-newcap;
        };
        void remap_erased() {
            size_t oldaddy = 0;
            size_t oldsize = 0;
            for (auto& i : erased) {
                if (oldaddy == 0) {
                    oldaddy = i.first;
                    oldsize = i.second;
                } else if (oldaddy + oldsize == i.first) {
                    oldsize += i.second;
                    erased.erase(i.first);
                    erased[oldaddy] = oldsize;
                } else {
                    oldaddy = i.first;
                    oldsize = i.second;
                }
            }
        }
        size_t allocate(size_t size) {
            size_t newcap = size + sizeof(GCObjectHead);
            for (auto& i : erased) {
                if (i.second >= newcap) {
                    GCObjectHead* ret = (GCObjectHead*)(i.first + (size_t)data);
                    (*ret) = GCObjectHead();
                    erased.erase(i.first);
                    if (i.second > newcap) {
                        erased[i.first + newcap] = i.second - newcap;
                    }
                    return i.first;
                }
            }
            return allocatenew(size);
        }
        void erase(size_t obj) {
            erased[obj] = ((GCObjectHead*)(obj + (size_t)data))->getTotalSize() + sizeof(GCObjectHead);
        }
    public:
        ~GCAllocator() {
            free(data); //erase all objects
        }
    };
    void GCIterator::jumpiferased() {
        if (obj->erased.find(index) != obj->erased.end()) {
            index += obj->erased[index];
        }
    }
    GCIterator& GCIterator::operator++(int) {
        index += head->getTotalSize() + sizeof(GCObjectHead);
        jumpiferased();
        return *this;
    }
    size_t GCObjectHead::getTotalSize() {
        return size*arrlen;
    }
    class __RecursiveSelfReferenceFinder
    {
        friend class GCObjectHead;
        vector<GCObjectHead*> checked;
        GCObjectHead* tochk;
        long long refs = 0;
        __RecursiveSelfReferenceFinder() {};
        size_t start(GCObjectHead* ob) 
        {
            checked.clear();
            refs = 0;
            tochk = ob;
            findrecursive(ob);
            if (refs < 0) return 0;
            return refs;
        }
        void findrecursive(GCObjectHead* search)
        {
            if (std::find(checked.begin(), checked.end(), search) != checked.end()) return;
            refs-= search->Crefs - search->ref_to_object.size();
            checked.push_back(search);
            for (auto& i : search->ref_to_object)
            {
                if (i == tochk)
                {
                    refs++;
                }
                else
                {
                    findrecursive(i);
                }
            }
        }
    };
    size_t GCObjectHead::getSelfRefs() {
        return __RecursiveSelfReferenceFinder().start(this);
    }
    size_t GCObjectHead::getTotalRefs() {
        return Crefs;
    }
    size_t GCObjectHead::getActiveRefs() {
        return getTotalRefs()-getSelfRefs();
    }   
    template <typename T>
    class GCObject final : public GCObjectHead
    {
    public:
        GCObject() : GCObjectHead() {
            this->size = sizeof(T);
        };
        T data;
    };
    class GC final {
        GC() {}
        friend class GCObjectHead;
        template <typename T>
        friend class Object;
        friend class Generic;
        GCAllocator allocator;
    public:
        //make this a singleton
        static GC& getInstance() {
            static GC instance;
            return instance;
        }
        GC(const GC&) = delete;
        GC& operator=(const GC&) = delete;
        static void collect() {
            #ifdef GCDEBUG
            std::cout << "GC collect" << std::endl;
            #endif
            /*for (auto& i : GC::getInstance().objects)
            {
                GC_PRINTOBJ_REF(i);
                if (i->getActiveRefs() == 0) {
                    #ifdef GCDEBUG
                    std::cout << "GC delete object" << std::endl;
                    #endif
                    i->e();
                    delete i;
                    GC::getInstance().objects.erase(std::find(GC::getInstance().objects.begin(), GC::getInstance().objects.end(), i));
                }
            }*/
            GCIterator it = GCIterator(
                (size_t)GC::getInstance().allocator.data,
                GC::getInstance().allocator.capacity,
                &(GC::getInstance().allocator)
            );
            it.jumpiferased();
            if (it.isAtEnd()) return;
            while (true) {
                GCObjectHead& obj = *it;
                GC_PRINTOBJ_REF((&obj));
                if (obj.getActiveRefs() == 0) {
                    #ifdef GCDEBUG
                    std::cout << "GC delete object" << std::endl;
                    #endif
                    obj.e();
                    GC::getInstance().allocator.erase(it.index);
                }
                it++;
                if (it.isAtEnd()) break;
            }
            GC::getInstance().allocator.remap_erased();
        }
        static size_t newobj(size_t size) {
            return GC::getInstance().allocator.allocate(size);
        }
        static GCObjectHead* getObject(size_t obj) {
            return (GCObjectHead*)(obj + (size_t)GC::getInstance().allocator.data);
        }
    };
    template <typename T>
    class Object {
    protected:
        //size_t data = -1;
        void destruct() {
            #ifdef GCDEBUG
            std::cout<< "Object dereferenced" << std::endl;
            #endif
            if (data != nullptr) {
                long long l = memcheck();
                #ifdef GCDEBUG
                std::cout << "GC ptr destruct" << std::endl;
                #endif
                if (l != -1) {
                    data->ref_to_object.erase(std::find(data->ref_to_object.begin(), data->ref_to_object.end(), GC::getInstance().objects[l]));
                    GC::getInstance().objects[l]->object_to_ref.erase(std::find(GC::getInstance().objects[l]->object_to_ref.begin(), GC::getInstance().objects[l]->object_to_ref.end(), data));
                }
                data->Crefs--;
                GC_PRINTOBJ_REF(data);
            }
        }
        long long memcheck() {
            /*GCObjectHead* b = (GCObjectHead*)(&GC::getInstance().objects.begin()[0]);
            GCObjectHead* e = (GCObjectHead*)(&GC::getInstance().objects.end()[0]);
            if ((size_t)this > (size_t)b && (size_t)this < (size_t)e) {
                #ifdef GCDEBUG
                std::cout << "GC ptr memcheck" << std::endl;
                #endif
                return ((size_t)this-(size_t)b) / sizeof(GCObject<T>);
            }
            #ifdef GCDEBUG
            std::cout << "GC ptr memcheck failed" << std::endl;
            #endif
            return -1;*/
        }
        void do_mem() {
            long long l = memcheck();
            if (l != -1) {
                #ifdef GCDEBUG
                std::cout << "Found references to objects" << std::endl;
                #endif
                //is in vector
                if (std::find(data->ref_to_object.begin(), data->ref_to_object.end(), GC::getInstance().objects[l]) != data->ref_to_object.end()) {
                    #ifdef GCDEBUG
                    std::cout << "Already processed reference" << std::endl;
                    #endif
                    return;
                }
                data->ref_to_object.push_back(GC::getInstance().objects[l]);
                GC::getInstance().objects[l]->object_to_ref.push_back(data);
            }
        }
    public:
        Object() : data(nullptr) {}
        Object(std::nullptr_t) : data(nullptr) {}
        Object(const Object& o) {
            #ifdef GCDEBUG
            std::cout<< "GC object copy" << std::endl;
            #endif
            if (data == nullptr) return;
            data = o.data;
            data->Crefs++;
            #ifdef GCDEBUG
            std::cout << "GC new object increment ref" << std::endl;
            #endif
            do_mem();
            GC_PRINTOBJ_REF(data);
        }
        Object(const T o) {
            #ifdef GCDEBUG
            std::cout << "GC object move" << std::endl;
            #endif
            data = new GCObject<T>();
            data->data = std::move(o);
            data->Crefs++;
            GC::add(data);
            do_mem();
            GC_PRINTOBJ_REF(data);
        }
        ~Object() {
            destruct();
        }
        Object& operator=(const Object& o) {
            destruct();
            data = o.data;
            if (data == nullptr) return *this;
            data->Crefs++;
            do_mem();
            GC_PRINTOBJ_REF(data);
            return *this;
        }
        Object& operator=(const T& o) {
            destruct();
            data = new GCObject<T>();
            data->data = o;
            data->Crefs++;
            GC::add(data);
            do_mem();
            GC_PRINTOBJ_REF(data);
            return *this;
        }
        T& operator*() {
            return data->data;
        }
        T* operator->() {
            return &(data->data);
        }
        operator T*() {
            return &(data->data);
        }
        operator T&() {
            return data->data;
        }
        operator T() {
            return data->data;
        }
    };
}