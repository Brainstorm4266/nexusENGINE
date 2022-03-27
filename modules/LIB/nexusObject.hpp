#pragma once
#include <thread>
#include <string>
#include <vector>
using std::string;
using std::vector;
#define nType _nType*
#define nObj _nObj*
#define nnObj _nnObj*
#define snObj _snObj*
#define inObj _inObj*
#define MainInterpreter _MainInterpreter*
#define InternalException _InternalException*
#define BaseExcObject _BaseExcObject*
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
enum InternalExceptionTypes {
    UNKNOWN,
    MISSINGFUNC
};
class _nType;
class _nObj;
class Thread;
class _InternalException {
public:
    string excinfo;
    InternalExceptionTypes type = UNKNOWN;
    const bool internal = true;
    virtual void finalize() {
        this->finalized = true;
        delete this;//- Goodbye.
    }
    virtual void reuse() {};
    bool getFinalized() {
        return this->finalized;
    }
protected:
    bool finalized = false;
};
class _MainInterpreter {
public:
    vector<nObj> objs;
    vector<InternalException> excstack;
    bool newexc = false;
    nType basetypeclass,baseobjclass,baseexcclass;
    nObj nilnObj;
    nObj falsenObj;
    nObj truenObj;
    vector<Thread*> threads;
    void throwerr(InternalException ie) {
        this->newexc = true;
        this->excstack.push_back(ie);
    }
    void catcherr() {
        this->newexc = false;
    }
    void finishexc() {
        this->catcherr();
        for (auto i : this->excstack) {
            i->finalize();
        }
        this->excstack.clear();
    }
};
class BaseThread {
public:
    ~BaseThread() {
        if (this->thread != nullptr) delete this->thread;
    }
    bool start() {
        if (!this->alreadystarted) {
            this->thread = new std::thread(this->task,this);
            this->alreadystarted = true;
            return true;
        } 
        return false;
    };
    void run() {
        this->task(this);
    }
    void kill() {
        this->killed = true;
    }
    bool getKilled() {
        return this->killed;
    }
    void join() {
        this->thread->join();
    }
    void (*task)(BaseThread*);
private:
    std::thread* thread;
    bool alreadystarted = false;
    bool killed = false;
};
class Thread : public BaseThread {
public:
    Thread(MainInterpreter i) {this->i = i;}
    MainInterpreter i;
    
};
class _nObj {
public:
    nType base = nullptr;
    MainInterpreter i;
    _nObj(MainInterpreter i) {
        this->i = i;
        this->i->objs.push_back(this);
    }
    unsigned long long refcnt = 1;
    void incref() {
        this->refcnt++;
    }
    void decref() {
        this->refcnt--;
    }
};
class _inObj : public _nObj {
public:
    _inObj(MainInterpreter i) : _nObj(i) {};
    long long ival = 0;
};
class _snObj : public _nObj {
public:
    _snObj(MainInterpreter i) : _nObj(i) {};
    string sval;
};
class _nnObj : public _nObj {
public:
    _nnObj(MainInterpreter i) : _nObj(i) {};
    long double nval = 0;
};
class _BaseExcObject : public _nObj, public _InternalException {
public:
    const bool internal = false;
    void finalize() {
        if (!this->finalized) {
            this->finalized = true;
            this->decref();
        }
    }
    void reuse() {
        if (this->finalized) {
            this->incref();
            this->finalized = false;
        }
    }
};
class vanObj {
public:
    vector<nObj> va;
    MainInterpreter i;
};
class nTypeFuncWrap {
public:
    nObj operator() (vanObj l) {
        for (auto i : l.va) i->incref();
        if (this->v != nullptr) {
            nObj r = this->v(l);
            for (auto i : l.va) i->decref();
            return r;
        }
        for (auto i : l.va) i->decref();
        return nullptr;
    };
    nTypeFuncWrap() {};
    nTypeFuncWrap(nObj (*f)(vanObj)) {
        this->v = f;
    }
protected:
    nObj (*v) (vanObj) = nullptr;
};
#define wrapnFunc(code) nTypeFuncWrap([](vanObj v) {\
    vector<nObj>& va = v.va;\
    MainInterpreter i = v.i;\
    if (true) code\
    return v.i->nilnobj;\
})
#define unerrfunc(token) wrapnFunc({\
    InternalException ie = new _InternalException;ie->type = MISSINGFUNC;ie->excinfo = (string)"Object of type " + va[0]->nType->name + (string) (" does not support operator of type " STRINGIFY(token) ".")\
})
template<typename T>
T& avoidconstructor() {
    void* m = malloc(sizeof T);
    T t = &((T*)m);
    free(m);
    return t;
}
namespace BaseObjectFunctions {
    long long inthash(nObj)
};
class _nType : public _nObj {
public:
    string name;
    nType base = nullptr;
    vector<nType> bases;
    nTypeFuncWrap 
    
    add,sub,mul,div,pow,
    iadd,isub,imul,idiv,ipow,
    and,or,xor,inv,
    iand,ior,ixor,
    pos,neg,
    lnot,land,lor,lxor,
    ilnot,iland,ilor,ilxor,
    call,hash,inthash,
    newobj,initobj,delprepobj,delobj,
    getitem,setitem,length,contains,
    getattr,setattr,hasattr,dir, 
    neq,eq,lt,le,gt,ge,
    ilt,ile,igt,ige
    ;
    _nType(MainInterpreter i);
};
_nType::_nType(MainInterpreter i) : _nObj(i) {
    this->base = i->basetypeclass;
}