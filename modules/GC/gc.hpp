#include <vector>
#include <exception>
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <unordered_map>
#include <cstdlib>
#include <string>
#include <tuple>
#include <typeinfo>
#include <sstream>

#include "../LIB/Events.hpp"
#include "../colors.hpp"

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
/*namespace gc_old {
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
}*/
namespace gc {
    using namespace std;
    template <typename T>
    class Object;
    class Generic;
    class GC;
    inline void print_warning(string msg) {
        std::cout << COLOR_YELLOW "[GC WARN]: " << msg << COLOR_RESET << std::endl;
    }
    template <typename T>
    class GCObject;
    enum RefType {
        STRONG,
        SELF,
        WEAK
    };  
    class GCObjectHead
    {
    public:
        friend class GC;
        friend class GCAllocator;
        vector<size_t> object_to_ref;
        vector<size_t> ref_to_object;
        //ref types
        // strong when object to ref 
        // self when both
        unsigned long long Crefs = 0; //strong ref
        volatile size_t size = 0;
        volatile size_t arrlen = 1;
        size_t getTotalSize();
        size_t getTotalRefs();
        size_t getActiveRefs();
        size_t getSelfRefs();
        GCObjectHead(size_t size, size_t arrlen)
            : Crefs(0), size(size), arrlen(arrlen)
        {}
    };
    class GCIterator {
        friend class GC;
        friend class GCAllocator;
        template <typename T>
        friend class Object;
        GCObjectHead* head;
        GCAllocator* obj;
        size_t ind;
        size_t index;
        size_t end;
        GCIterator(size_t head, size_t end, GCAllocator* obj) :
            head((GCObjectHead*)head),
            index(0),
            end(end),
            obj(obj),
            ind(0)
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
        inline void jumpiferased();
    };
    class GCAllocator {
        friend class GC;
        friend class GCIterator;
        template <typename T>
        friend class Object;
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
            char* ptr = (char*)(capacity-newcap + (size_t)data);
            GCObjectHead&& head = GCObjectHead(size,1);
            memcpy(ptr, &head, newcap);
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
                    (*ret) = GCObjectHead(size,1);
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
            ++ind;
        }
    }
    GCIterator& GCIterator::operator++(int) {
        index += head->getTotalSize() + sizeof(GCObjectHead);
        ++ind;
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
        void findrecursive(GCObjectHead* search);
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
        GCObject() : GCObjectHead(sizeof(T),1) {}
        T data;
    };
    class GC final {
        GC() {}
        friend class GCObjectHead;
        template <typename T>
        friend class Object;
        friend class Generic;
        friend class __RecursiveSelfReferenceFinder;
        GCAllocator allocator;
        vector<tuple<size_t, void*>> mark_to_delete;
        
        static size_t newobj(size_t size) {
            return GC::getInstance().allocator.allocate(size);
        }
        static GCObjectHead* getObject(size_t obj) {
            return (GCObjectHead*)(obj + (size_t)GC::getInstance().allocator.data);
        }
        static void mark(size_t obj, void* obj_) {
            GC::getInstance().mark_to_delete.push_back(make_tuple(obj, obj_));
        }
    public:
        //make this a singleton
        static GC& getInstance() {
            static GC instance;
            return instance;
        }
        GC(const GC&) = delete;
        GC& operator=(const GC&) = delete;
        static void fullcollect() {
            #ifdef GCDEBUG
            std::cout << "GC fullcollect" << std::endl;
            #endif
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
                    for (size_t& o : obj.ref_to_object) {
                        GC::getObject(o)->object_to_ref.erase(std::find(GC::getObject(o)->object_to_ref.begin(), GC::getObject(o)->object_to_ref.end(), it.index));
                    }
                    for (size_t& o : obj.object_to_ref) {
                        GC::getObject(o)->ref_to_object.erase(std::find(GC::getObject(o)->ref_to_object.begin(), GC::getObject(o)->ref_to_object.end(), it.index));
                    }
                    GC::getInstance().allocator.erase(it.index);
                }
                it++;
                if (it.isAtEnd()) break;
            }
            GC::getInstance().allocator.remap_erased();
        }
        static void collect() {
            #ifdef GCDEBUG
            std::cout << "GC collect" << std::endl;
            #endif
            for (const auto& i : GC::getInstance().mark_to_delete) {
                size_t obj = get<0>(i);
                size_t sv = GC::getObject(obj)->getActiveRefs();
                if (sv != 0) {
                    stringstream st;
                    stringstream sts;
                    st << "0x" << std::hex << (size_t)get<1>(i);
                    sts << "0x" << std::hex << (size_t)GC::getObject(obj);
                    print_warning("Object pointer " + st.str() + " had some bullshit references, object " + sts.str() + " is still in use with " + std::to_string(sv) + " references.");
                } else {
                    #ifdef GCDEBUG
                    std::cout << "GC delete object" << std::endl;
                    #endif
                    for (size_t& o : GC::getObject(obj)->ref_to_object) {
                        GC::getObject(o)->object_to_ref.erase(std::find(GC::getObject(o)->object_to_ref.begin(), GC::getObject(o)->object_to_ref.end(), obj));
                    }
                    for (size_t& o : GC::getObject(obj)->object_to_ref) {
                        GC::getObject(o)->ref_to_object.erase(std::find(GC::getObject(o)->ref_to_object.begin(), GC::getObject(o)->ref_to_object.end(), obj));
                    }
                    GC::getInstance().allocator.erase(obj);
                }
            }
            GC::getInstance().allocator.remap_erased();
        }
    };
    void __RecursiveSelfReferenceFinder::findrecursive(GCObjectHead* search) {
            if (std::find(checked.begin(), checked.end(), search) != checked.end()) return;
            refs-= search->Crefs - search->ref_to_object.size();
            checked.push_back(search);
            for (auto& i : search->ref_to_object)
            {
                if (GC::getObject(i) == tochk)
                {
                    refs++;
                }
                else
                {
                    findrecursive(GC::getObject(i));
                }
            }
        }
    template <typename T>
    class Object {
    protected:
        size_t data = -1;
        void destruct() {
            #ifdef GCDEBUG
            std::cout<< "Object dereferenced" << std::endl;
            #endif
            if (data != -1) {
                long long l = memcheck();
                #ifdef GCDEBUG
                std::cout << "GC ptr destruct" << std::endl;
                #endif
                if (l != -1) {
                    GC::getObject(data)->ref_to_object.erase(std::find(GC::getObject(data)->ref_to_object.begin(), GC::getObject(data)->ref_to_object.end(), l));
                    GC::getObject(l)->object_to_ref.erase(std::find(GC::getObject(l)->object_to_ref.begin(), GC::getObject(l)->object_to_ref.end(), data));
                }
                GC::getObject(data)->Crefs--;
                GC_PRINTOBJ_REF(GC::getObject(data));
                if (GC::getObject(data)->getActiveRefs() == 0) {
                    #ifdef GCDEBUG
                    std::cout << "GC delete mark object" << std::endl;
                    #endif
                    GC::mark(data, this);
                }
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
            size_t b = (size_t)GC::getObject(0);
            size_t e = (size_t)GC::getObject(GC::getInstance().allocator.capacity);
            if ((size_t)this > b && (size_t)this < e) {
                #ifdef GCDEBUG
                std::cout << "GC ptr memcheck" << std::endl;
                #endif
                //continue here
                GCIterator it = GCIterator(
                    (size_t)GC::getInstance().allocator.data,
                    GC::getInstance().allocator.capacity,
                    &(GC::getInstance().allocator)
                );
                it.jumpiferased();
                while (!it.isAtEnd()) {
                    if ((size_t)((GCObjectHead*)it) > (size_t)this && (it->size + (size_t)it.head) < (size_t)this) {
                        return it.index;
                    }
                    it++;
                }
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
                if (std::find(GC::getObject(data)->ref_to_object.begin(), GC::getObject(data)->ref_to_object.end(), l) != GC::getObject(data)->ref_to_object.end()) {
                    #ifdef GCDEBUG
                    std::cout << "Already processed reference" << std::endl;
                    #endif
                    return;
                }
                GC::getObject(data)->ref_to_object.push_back(l);
                GC::getObject(l)->object_to_ref.push_back(data);
            }
        }
    public:
        Object() : data(-1) {}
        Object(std::nullptr_t) : data(-1) {}
        Object(const Object& o) {
            #ifdef GCDEBUG
            std::cout<< "GC object copy" << std::endl;
            #endif
            if (data == -1) return;
            data = o.data;
            GC::getObject(data)->Crefs++;
            #ifdef GCDEBUG
            std::cout << "GC new object increment ref" << std::endl;
            #endif
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
        }
        Object(const T& o) {
            #ifdef GCDEBUG
            std::cout << "GC object move" << std::endl;
            #endif
            data = GC::getInstance().allocator.allocate(sizeof(T));
            GC::getObject(data)->Crefs = 1;
            ((GCObject<T>*)GC::getObject(data))->data = o;
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
        }
        ~Object() {
            destruct();
            if (data != -1) {
                if (GC::getObject(data)->getActiveRefs() == -1) GC::mark(data,this);
            }
        }
        Object& operator=(const Object& o) {
            destruct();
            data = o.data;
            if (data == -1) return *this;
            GC::getObject(data)->Crefs++;
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
            return *this;
        }
        Object& operator=(const T& o) {
            destruct();
            data = GC::getInstance().allocator.allocate(sizeof(T));
            GC::getObject(data)->Crefs = 1;
            ((GCObject<T>*)GC::getObject(data))->data = o;
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
            return *this;
        }
        Object& operator=(std::nullptr_t) {
            destruct();
            data = -1;
            return *this;
        }
        T& operator*() {
            return ((GCObject<T>*)GC::getObject(data))->data;
        }
        T* operator->() {
            return &((GCObject<T>*)GC::getObject(data))->data;
        }
        operator T*() {
            if (data == -1) return nullptr;
            return &((GCObject<T>*)GC::getObject(data))->data;
        }
        //operator T&() {
        //    if (data == -1) throw std::runtime_error("Null pointer dereference");
        //    return ((GCObject<T>*)GC::getObject(data))->data;
        //}
        operator T() {
            if (data == -1) throw std::runtime_error("Null pointer dereference");
            return ((GCObject<T>*)GC::getObject(data))->data;
        }
        std::ostream& operator << (std::ostream& co) {
            co << ((GCObject<T>*)GC::getObject(data))->data;
            return co;
        }
    };
    class Generic final : protected Object<void*> {
    public:
        Generic() : Object<void*>() {}
        Generic(std::nullptr_t) : Object<void*>() {}
        Generic(const Generic& o) : Object<void*>() {
            #ifdef GCDEBUG
            std::cout<< "GC object copy" << std::endl;
            #endif
            if (data == -1) return;
            data = o.data;
            GC::getObject(data)->Crefs++;
            #ifdef GCDEBUG
            std::cout << "GC new object increment ref" << std::endl;
            #endif
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
        }
        template <typename T>
        Generic(const Object<T>& o) : Object<void*>() {
            #ifdef GCDEBUG
            std::cout << "GC object copy" << std::endl;
            #endif
            data = o.data;
            GC::getObject(data)->Crefs++;
            #ifdef GCDEBUG
            std::cout << "GC new object increment ref" << std::endl;
            #endif
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
        }
        template <typename T>
        Generic(const T& o) : Object<void*>() {
            #ifdef GCDEBUG
            std::cout << "GC object move" << std::endl;
            #endif
            data = GC::allocator.allocate(sizeof(T));
            GC::getObject(data)->Crefs = 1;
            ((GCObject<T>*)GC::getObject(data))->data = o;
            do_mem();
            GC_PRINTOBJ_REF(data);
        }
        Generic& operator=(const Generic& o) {
            destruct();
            data = o.data;
            if (data == -1) return *this;
            GC::getObject(data)->Crefs++;
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
            return *this;
        }
        template <typename T>
        Generic& operator=(const Object<T>& o) {
            destruct();
            data = o.data;
            if (data == -1) return *this;
            GC::getObject(data)->Crefs++;
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
            return *this;
        }
        template <typename T>
        Generic& operator=(const T& o) {
            destruct();
            data = GC::allocator.allocate(sizeof(T));
            GC::getObject(data)->Crefs = 1;
            ((GCObject<T>*)GC::getObject(data))->data = o;
            do_mem();
            GC_PRINTOBJ_REF(GC::getObject(data));
            return *this;
        }
        Generic& operator=(std::nullptr_t) {
            destruct();
            data = -1;
            return *this;
        }
        template <typename T>
        T& try_cast() {
            if (data == -1) throw std::runtime_error("Null pointer dereference");
            if (GC::getObject(data)->size != sizeof(T)) throw std::bad_cast("Invalid cast");
            return ((GCObject<T>*)GC::getObject(data))->data;
        }
    };
}