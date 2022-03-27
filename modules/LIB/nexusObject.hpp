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
    MISSINGFUNC,
    INVALIDNUMOFARGS
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
    nType basetypeclass;
    nType baseobjclass;
    nType baseexcclass;
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
#define wrapnFunc(code,argnum) nTypeFuncWrap([](vanObj v) {\
    vector<nObj>& va = v.va;\
    \
    MainInterpreter i = v.i;\
    if (true) code\
    return v.i->nilnobj;\
})
#define unerrfunc(token) wrapnFunc({\
    InternalException ie = new _InternalException;ie->type = MISSINGFUNC;ie->excinfo = (string)"Object of type " + va[0]->base->name + (string)(" does not support operator " STRINGIFY(token) ".");i->throw(ie);\
})
#define bierrfunc(token) wrapnFunc({\
    InternalException ie = new _InternalException;ie->type = MISSINGFUNC;ie->excinfo = (string)"Object of type " + va[0]->base->name + (string)(" does not support operator " STRINGIFY(token) " with argument of type ") + va[1]->base->name + (string)".";i->throw(ie); \
})
template<typename T>
T& avoidconstructor() {
    void* m = malloc(sizeof T);
    T t = &((T*)m);
    free(m);
    return t;
}
namespace BaseObjFuncs {
    nTypeFuncWrap add = bierrfunc("add");
    nTypeFuncWrap sub = bierrfunc("sub");
    nTypeFuncWrap mul = bierrfunc("mul");
    nTypeFuncWrap div = bierrfunc("div");
    nTypeFuncWrap pow = bierrfunc("pow");
    nTypeFuncWrap mod = bierrfunc("mod");
    nTypeFuncWrap iadd = bierrfunc("iadd");
    nTypeFuncWrap isub = bierrfunc("isub");
    nTypeFuncWrap imul = bierrfunc("imul");
    nTypeFuncWrap idiv = bierrfunc("idiv");
    nTypeFuncWrap ipow = bierrfunc("ipow");
    nTypeFuncWrap imod = bierrfunc("imod");
    nTypeFuncWrap and = bierrfunc("and");
    nTypeFuncWrap or = bierrfunc("or");
    nTypeFuncWrap xor = bierrfunc("xor");
    nTypeFuncWrap inv = bierrfunc("inv");
    nTypeFuncWrap shl = bierrfunc("shl");
    nTypeFuncWrap shr = bierrfunc("shr");
    nTypeFuncWrap and = bierrfunc("and");
    nTypeFuncWrap or = bierrfunc("or");
    nTypeFuncWrap xor = bierrfunc("xor");
    nTypeFuncWrap inv = bierrfunc("inv");
    nTypeFuncWrap shl = bierrfunc("shl");
    nTypeFuncWrap shr = bierrfunc("shr");
    nTypeFuncWrap iand = bierrfunc("iand");
    nTypeFuncWrap ior = bierrfunc("ior");
    nTypeFuncWrap ixor = bierrfunc("ixor");
    nTypeFuncWrap ishl = bierrfunc("ishl");
    nTypeFuncWrap ishr = bierrfunc("ishr");
    
};
class _nType : public _nObj {
public:
    string name;
    nType base = nullptr;
    vector<nType> bases;
    nTypeFuncWrap 
    
    add,sub,mul,div,pow,mod,
    iadd,isub,imul,idiv,ipow,imod,
    and,or,xor,inv,shl,shr,
    iand,ior,ixor,ishl,ishr,
    pos,neg,
    lnot,land,lor,lxor,
    iland,ilor,ilxor,
    call,hash,repr,
    toint,tonum,tostr,
    newobj,initobj,delprepobj,delobj,
    getitem,setitem,length,contains,
    getattr,setattr,hasattr,dir, 
    neq,eq,lt,le,gt,ge,
    ilt,ile,igt,ige
    ;
    static long long inthash(nObj o) {
        o->incref();
        vanObj v;
        v.i = o->i;
        v.va.push_back(o);
        inObj h = (inObj)(o->base->hash(v));
        long long r = h->ival;
        h->decref();
        o->decref();
        return r;
    }
    _nType(MainInterpreter i);
};
_nType::_nType(MainInterpreter i) : _nObj(i) {
    this->base = i->basetypeclass;
}