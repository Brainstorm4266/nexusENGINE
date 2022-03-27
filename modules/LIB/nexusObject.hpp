#pragma once

#include <thread>
#include <string>
#include <vector>
#include <iomanip>
#include <unordered_map>

#include "__hash.hpp"

using std::string;
using std::vector;
using std::stringstream;
using std::unordered_map;

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

class _nType;
class _nObj;
class Thread;

enum InternalExceptionTypes {
    UNKNOWN,
    MISSINGFUNC,
    INVALIDNUMOFARGS
};
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
    nType basetypeclass;
    nType baseobjclass;
    nType baseexcclass;
    nType intclass;
    nType strclass;
    nType numclass;
    nObj nilnObj;
    nObj falsenObj;
    nObj truenObj;
    vector<Thread*> threads;
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
class _nObj {
public:
    nType base = nullptr;
    const bool has__dict__ = false;
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
class Thread : public BaseThread, public _nObj {
public:
    vector<InternalException> excstack;
    bool newexc = false;
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
class _inObj : public _nObj {
public:
    _inObj(MainInterpreter i) : _nObj(i) {this->base = i->intclass;};
    long long ival = 0;
};
class _snObj : public _nObj {
public:
    _snObj(MainInterpreter i) : _nObj(i) {this->base = i->strclass;};
    string sval;
};
class _nnObj : public _nObj {
public:
    _nnObj(MainInterpreter i) : _nObj(i) {this->base = i->numclass;};
    long double nval = 0;
};
class _vnObj : public _nObj {
public:
    const bool has__dict__ = true;
    unordered_map<long long,nObj> __dict__;
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
    Thread* t;
    void del() {
        for (auto i : va) {
            i->decref();
        }
    }
    void init() {
        for (auto i : va) {
            i->incref();
        }
    }
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
    Thread* t = v.t;\
    MainInterpreter i = v.t->i;\
    v.init();\
    if (true) code\
    v.del();\
    return i->nilnObj;\
})
#define unerrfunc(token) wrapnFunc({\
    InternalException ie = new _InternalException;ie->type = MISSINGFUNC;ie->excinfo = (string)"Object of type " + va[0]->base->name + (string)(" does not support operator " token ".");t->throwerr(ie);\
})
#define bierrfunc(token) wrapnFunc({\
    InternalException ie = new _InternalException;ie->type = MISSINGFUNC;ie->excinfo = (string)"Object of type " + va[0]->base->name + (string)(" does not support operator " token " with argument of type ") + va[1]->base->name + (string)".";t->throwerr(ie); \
})
template<typename T>
T& avoidconstructor() {
    void* m = malloc(sizeof T);
    T t = &((T*)m);
    free(m);
    return t;
}
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
    static long long inthash(nObj o, Thread* t) {
        vanObj v;
        v.t = t;
        v.va.push_back(o);
        inObj h = (inObj)(o->base->hash(v));
        long long r = h->ival;
        h->decref();
        return r;
    }
    _nType(MainInterpreter i);
};
namespace __hash {
    hash_f<_nObj> ho;
    hash_f<_nType> ht;
    unsigned long long hashobj(nObj obj) {
        obj->incref();
        obj->base->incref();
        unsigned long long hor = ho._Do_hash(*(obj));
        unsigned long long htr = ht._Do_hash(*(obj->base));
        obj->decref();
        obj->base->decref();
        return inthash(hor) << 32 + inthash(htr);
    }
};
template<typename T>
inline std::string ptr_to_hex( T* i )
{
  std::stringstream stream;
  stream << "0x" << std::hex << i;
  return stream.str();
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
    nTypeFuncWrap pos = unerrfunc("pos");
    nTypeFuncWrap neg = unerrfunc("neg");
    nTypeFuncWrap lnot = unerrfunc("lnot");
    nTypeFuncWrap land = bierrfunc("land");
    nTypeFuncWrap lor = bierrfunc("lor");
    nTypeFuncWrap lxor = bierrfunc("lxor");
    nTypeFuncWrap iland = bierrfunc("iland");
    nTypeFuncWrap ilor = bierrfunc("ilor");
    nTypeFuncWrap ilxor = bierrfunc("ilxor");
    nTypeFuncWrap call = unerrfunc("call");
    nTypeFuncWrap hash = wrapnFunc({
        long long h = __hash::hashobj(va[0]);
        inObj in = new _inObj(i);
        in->ival = h;
        return (nObj)in;
    });
    nTypeFuncWrap repr = wrapnFunc({
        snObj s = new _snObj(i);
        s->sval = "<object at " + ptr_to_hex(va[0]) + " of type " + va[0]->base->name + ">";
        return (nObj)s;
    });
    nTypeFuncWrap toint = unerrfunc("toint");
    nTypeFuncWrap tonum = unerrfunc("tonum");
    nTypeFuncWrap tostr = repr;
    nTypeFuncWrap newobj = wrapnFunc({
        return new _nObj(i);
    });
    nTypeFuncWrap initobj = wrapnFunc({});
    nTypeFuncWrap delpreobj = wrapnFunc({});
    nTypeFuncWrap delobj = wrapnFunc({delete va[0];});
    nTypeFuncWrap getitem = unerrfunc("getitem");
    nTypeFuncWrap setitem = unerrfunc("setitem");
    nTypeFuncWrap length = unerrfunc("length");
    nTypeFuncWrap contains = unerrfunc("contains");
    #error getattr missing, also the other base object functions
    nTypeFuncWrap getattr = wrapnFunc({

    });
};
_nType::_nType(MainInterpreter i) : _nObj(i) {
    this->base = i->basetypeclass;
}