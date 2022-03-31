#pragma once

#include <thread>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "__hash.hpp"
#include "isInVector.hpp"
#include "stringUtilities.hpp"

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
#define funcnObj _funcnObj*
#define dnObj _dnObj*
#define vnObj _vnObj*

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

class _nType;
class _nObj;
class Thread;

template <typename T,typename T2>
bool check_key(unordered_map<T, T2> m, T key)
{
    if (m.find(key) == m.end())
        return false;
 
    return true;
}

enum InternalExceptionTypes {
    UNKNOWN,
    MISSINGFUNC,
    INVALIDNUMOFARGS,
    INVALIDARG,
    ATTRNOTFOUND
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
    nType boolclass;
    nType funcclass;
    nType dictclass;
    nType threadclass;
    nType listclass;
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
    _nObj(MainInterpreter i){
        this->i = i;
        this->i->objs.push_back(this);
        this->base = i->baseobjclass;
    };
    ~_nObj() {
        reinterpret_cast<nObj>(this->base)->decref();
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
    Thread(MainInterpreter i) : _nObj(i) {
        this->i = i;
        this->i->threads.push_back(this);
        this->base = i->threadclass;
        reinterpret_cast<nObj>(this->base)->incref();
    };
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
    _inObj(MainInterpreter i) : _nObj(i) {this->base = i->intclass;reinterpret_cast<nObj>(this->base)->incref();};
    long long ival = 0;
};
class _snObj : public _nObj {
public:
    _snObj(MainInterpreter i) : _nObj(i) {this->base = i->strclass;reinterpret_cast<nObj>(this->base)->incref();};
    string sval;
};
class _nnObj : public _nObj {
public:
    _nnObj(MainInterpreter i) : _nObj(i) {this->base = i->numclass;reinterpret_cast<nObj>(this->base)->incref();};
    long double nval = 0;
};
struct dictkey {
    string key;
    unsigned long long v;
    static dictkey& createfromstring(string s, MainInterpreter i);
    static dictkey& createfromobj(snObj o);
};
class _dnObj : public _nObj {
public:
    _dnObj(MainInterpreter i,unordered_map<dictkey,nObj>& dict) : _nObj(i) {this->base = i->dictclass;this->dict = dict;reinterpret_cast<nObj>(this->base)->incref();};
    _dnObj(MainInterpreter i) : _nObj(i) {this->base = i->dictclass;reinterpret_cast<nObj>(this->base)->incref();};
    unordered_map<dictkey,nObj> dict;
};
class _lnObj : public _nObj {
public:
    _lnObj(MainInterpreter i) : _nObj(i) {this->base = i->listclass;reinterpret_cast<nObj>(this->base)->incref();};
    vector<nObj> lval;
};
class _vnObj : public _nObj {
public:
    const bool has__dict__ = true;
    _vnObj(MainInterpreter i) : _nObj(i) {this->__dict__ = new _dnObj(i);this->__dict__->incref();reinterpret_cast<nObj>(this->base)->incref();}
    ~_vnObj() {
        this->__dict__->decref();
    }
    dnObj __dict__;
};
class _BaseExcObject : public _nObj, public _InternalException {
public:
    _BaseExcObject(MainInterpreter i) : _nObj(i) {this->base = i->baseexcclass;reinterpret_cast<nObj>(this->base)->incref();};
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
    virtual nObj operator() (vanObj l) {
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
    nTypeFuncWrap(nObj (*f)(vanObj), bool isnull = false) {
        this->v = f;
        this->null = isnull;
    }
    bool isNull() {
        return this->null || this->v == nullptr;
    };
protected:
    bool null = true;
    nObj (*v) (vanObj) = nullptr;
};
class _funcnObj : public _nObj {
public:
    _funcnObj(MainInterpreter i) : _nObj(i) {this->base = i->funcclass;reinterpret_cast<nObj>(this->base)->incref();}
    _funcnObj(MainInterpreter i, nTypeFuncWrap f) : _nObj(i) {this->base = i->funcclass; this->f = f;reinterpret_cast<nObj>(this->base)->incref();}
    nTypeFuncWrap f;
    nObj operator()(vanObj v) {
        return this->f.operator()(v);
    }
    bool isNull() {
        return this->f.isNull();
    };
};
#define throwInternalException(typeexc,info,thread) if (true) {InternalException ie = new _InternalException;ie->type = typeexc;ie->excinfo = info;thread->throwerr(ie);}
#define wrapnFunc_w_argn(code,argnum) nTypeFuncWrap([](vanObj v) {\
    if (v.va.size() != argnum) throwInternalException(INVALIDNUMOFARGS,(string)"Expected " + std::to_string(argnum) + (string)" arguments but got " + std::to_string(v.va.size()),v.t)\
    vector<nObj>& va = v.va;\
    Thread* t = v.t;\
    MainInterpreter i = v.t->i;\
    v.init();\
    if (true) code\
    v.del();\
    return i->nilnObj;\
})
#define wrapnFunc(code) nTypeFuncWrap([](vanObj v) {\
    vector<nObj>& va = v.va;\
    Thread* t = v.t;\
    MainInterpreter i = v.t->i;\
    v.init();\
    if (true) code\
    v.del();\
    return i->nilnObj;\
})
#define wrapnFuncnull(code) nTypeFuncWrap([](vanObj v) {\
    vector<nObj>& va = v.va;\
    Thread* t = v.t;\
    MainInterpreter i = v.t->i;\
    v.init();\
    if (true) code\
    v.del();\
    return i->nilnObj;\
},true)
#define unerrfunc(token) wrapnFuncnull({\
    throwInternalException(MISSINGFUNC,(string)"Object of type " + va[0]->base->name + (string)(" does not support operator " token "."),t)\
})
#define bierrfunc(token) wrapnFuncnull({\
    throwInternalException(MISSINGFUNC,((string)"Object of type " + va[0]->base->name + (string)(" does not support operator " token " with argument of type ") + va[1]->base->name + (string)"."),t)\
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
    vector<nType> mro;
    bool isinternal = false;
    void redoMRO() {
        this->mro.clear();
        this->mro.push_back(this);
        for (auto i : this->bases) {
            this->mro.insert(this->mro.end(),i->mro.begin(),i->mro.end());
        }
        this->mro.erase(std::unique(this->mro.begin(),this->mro.end()),this->mro.end());
    }
    /*nTypeFuncWrap 
    
    add,sub,mul,div,pow,mod,
    iadd,isub,imul,idiv,ipow,imod,
    andf,orf,xorf,inv,shl,shr,
    iand,ior,ixor,ishl,ishr,
    pos,neg,
    lnot,land,lor,lxor,
    iland,ilor,ilxor,
    call,hash,repr,
    toint,tonum,tostr,
    newobj,initobj,delprepobj,delobj,
    getitem,setitem,length,contains,delitem,
    getattr,setattr,hasattr,dir,delattr,
    neq,eq,lt,le,gt,ge,
    ilt,ile,igt,ige,
    iter,next
    ;*/
    unordered_map<string,nTypeFuncWrap> funcs;
    static long long inthash(nObj o, Thread* t) {
        vanObj v;
        v.t = t;
        v.va.push_back(o);
        inObj h = (inObj)(o->base->funcs["hash"](v));
        long long r = h->ival;
        h->decref();
        return r;
    }
    static nObj intgetattr(nObj o, string attr, Thread* t) {
        vanObj v;
        v.t = t;
        v.va.push_back(o);
        snObj s = new _snObj(o->i);
        s->sval = attr;
        v.va.push_back(s);
        nObj r = o->base->funcs["getattr"](v);
        s->decref();
        return r;
    }
    nTypeFuncWrap& intgetop(string attr);
    _nType(MainInterpreter i);
};
namespace __hash {
    hash_f<_nObj> ho;
    hash_f<_nType> ht;
    unsigned long long hashobj(nObj obj) {
        obj->incref();
        obj->base->incref();
        unsigned long long hor = ho._Do_hash(*(obj)); //ignore this error
        unsigned long long htr = ht._Do_hash(*(obj->base)); //ignore this error
        obj->decref();
        obj->base->decref();
        return inthash(hor,htr);
    }
};
template<typename T>
inline std::string ptr_to_hex( T* i )
{
  std::stringstream stream;
  stream << "0x" << std::hex << i;
  return stream.str();
}
class nTypeFuncWrapR : public nTypeFuncWrap {
public:
    nTypeFuncWrapR(nObj o) : nTypeFuncWrap() {
        this->o = o;
        o->incref();
    };

    nObj operator()(vanObj v) {
        vanObj vo;
        vo.t = v.t;
        vo.va = v.va;
        vo.va.insert(vo.va.begin(),o);
        return this->o->base->funcs["call"](v);
    }
    ~nTypeFuncWrapR() {
        o->decref();
    }
protected:
    nObj o;
};
#define isofType(o,cls) (o->base == cls || isInVector(o->base->mro,cls))
#define isnType(o) (o->base == i->basetypeclass || isInVector(o->base->mro,i->basetypeclass))
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
    nTypeFuncWrap andf = bierrfunc("and"); //and
    nTypeFuncWrap orf = bierrfunc("or");   //or
    nTypeFuncWrap xorf = bierrfunc("xor"); //xor
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
    nTypeFuncWrap toint = unerrfunc("toint"); //int
    nTypeFuncWrap tonum = unerrfunc("tonum"); //num
    nTypeFuncWrap tostr = repr;               //str
    nTypeFuncWrap newobj = wrapnFunc({        //new
        if (va.size() < 1) throwInternalException(INVALIDNUMOFARGS,"Expected 1 arguments but got 0",t);
        if (!isnType(va[0])) throwInternalException(INVALIDARG,"Expected argument 1 of type " + i->basetypeclass->name + " but got " + va[0]->base->name,t);
        nObj o = new _nObj(i);
        o->base = (nType)va[0];
        va[0]->incref();
        return o;
    });
    nTypeFuncWrap initobj = wrapnFunc({}); //init
    nTypeFuncWrap delpreobj = wrapnFunc({}); //del
    nTypeFuncWrap delobj = wrapnFunc({va[0]->base->decref();delete va[0];}); //internal use only, del_int
    nTypeFuncWrap getitem = unerrfunc("getitem");
    nTypeFuncWrap setitem = unerrfunc("setitem");
    nTypeFuncWrap delitem = unerrfunc("delitem");
    nTypeFuncWrap length = unerrfunc("length");
    nTypeFuncWrap contains = unerrfunc("contains");
    #error dir missing, also the other base object functions
    #define __internal_getattr__(obj,attrname) if (attrname == "__type__") return (nObj)(obj->base);\
    else if (attrname == "__dict__" && obj->has__dict__) return (nObj)(((vnObj)obj)->__dict__);
    #define __getattr_cls__(cls,attrname) if (check_key(((nType)cls)->funcs,attrname)) if (!(((nType)cls)->funcs[attrname].isNull())) return (nObj)(new _funcnObj(cls->i,((nType)cls)->funcs[attrname]));
    nTypeFuncWrap getattr = wrapnFunc({
        if (va.size() != 2) throwInternalException(INVALIDNUMOFARGS,"Expected 2 arguments but got " + std::to_string(va.size()),t);
        if (!isofType(va[1],i->strclass)) throwInternalException(INVALIDARG,"Expected argument 2 of type " + i->strclass->name + " but got " + va[1]->base->name,t);
        //ok
        bool isSpecialMethod = (startsWith(((snObj)(va[1]))->sval,"__") && endsWith(((snObj)(va[1]))->sval,"__"));
        string attrname = ((snObj)(va[1]))->sval;
        bool isType = isnType(va[0]);
        dictkey h = dictkey::createfromobj((snObj)va[1]);
        __internal_getattr__(va[0],attrname);
        if (va[0]->has__dict__) if (check_key(((vnObj)va[0])->__dict__->dict,h)) return ((vnObj)va[0])->__dict__->dict[h];
        if (isSpecialMethod && isType) {
            __getattr_cls__(va[0],attrname);
        } else if (isSpecialMethod) {
            for (nType i : va[0]->base->mro) {
                __getattr_cls__(i,attrname);
            }
            if (check_key(va[0]->base->funcs,attrname)) {
                return (nObj)(new _funcnObj(va[0]->base->i,va[0]->base->funcs[attrname]));
            }
        }
        for (nType i : va[0]->base->mro) {
            if (i->has__dict__) {
                unsigned long long h = va[1]->base->inthash(va[1],t);
                if (check_key(((vnObj)i)->__dict__->dict,h)) {
                    return (nObj)(((vnObj)i)->__dict__->dict[h]);
                }
            }
        }
        throwInternalException(ATTRNOTFOUND,"Attribute " + attrname + " not found",t);
    });
    nTypeFuncWrap setattr = wrapnFunc({
        if (va.size() != 3) throwInternalException(INVALIDNUMOFARGS,"Expected 3 arguments but got " + std::to_string(va.size()),t);
        if (!isofType(va[1],i->strclass)) throwInternalException(INVALIDARG,"Expected argument 2 of type " + i->strclass->name + " but got " + va[1]->base->name,t);
        bool isSpecialMethod = (startsWith(((snObj)(va[1]))->sval,"__") && endsWith(((snObj)(va[1]))->sval,"__"));
        string attrname = ((snObj)(va[1]))->sval;
        if (isnType(va[0])) if (((nType)va[0])->isinternal) throwInternalException(ATTRNOTFOUND,"Cannot set attribute for internal class "+ ((nType)va[0])->name,t);
        if (attrname == "__type__") throwInternalException(ATTRNOTFOUND,"Attribute __type__ cannot be set",t);
        if (attrname == "__dict__") {
            if (va[0]->has__dict__ && isofType(va[2],i->dictclass)) {
                ((vnObj)va[0])->__dict__->decref();
                ((vnObj)va[0])->__dict__ = (dnObj)va[2];
                va[2]->incref();
            } else if (isofType(va[2],i->dictclass)) {
                throwInternalException(ATTRNOTFOUND,"Attribute __dict__ cannot be set",t);
            }
            else {
                throwInternalException(INVALIDARG,"Expected argument 3 of type " + i->dictclass->name + " but got " + va[2]->base->name,t);
            }
        }
        if (isSpecialMethod) {
            if (!va[2]->base->intgetop("call").isNull()) throwInternalException(INVALIDARG,"Expected argument 3 to be callable",t);
        }
        if (isSpecialMethod && isnType(va[0])) {
            if (!check_key(((nType)va[0])->funcs,attrname)) {
                throwInternalException(ATTRNOTFOUND,"Attribute " + attrname + " cannot be set",t);
            }
            if (attrname == "del_int") throwInternalException(ATTRNOTFOUND,"Attribute del_int cannot be set",t);
            ((nType)va[0])->funcs[attrname] = nTypeFuncWrapR(va[2]);
        }
        else if (isSpecialMethod && check_key(va[0]->base->funcs,attrname)) {
            if (attrname == "del_int") throwInternalException(ATTRNOTFOUND,"Attribute del_int cannot be set",t);
            va[0]->base->funcs[attrname] = nTypeFuncWrapR(va[2]);
        }
        if (va[0]->has__dict__) {
            unsigned long long h = va[1]->base->inthash(va[1],t);
            if (check_key(((vnObj)va[0])->__dict__->dict,h)) {
                ((vnObj)va[0])->__dict__->dict[h]->decref();
            }
            ((vnObj)va[0])->__dict__->dict[h] = va[2];
            va[2]->incref();
        }
        throwInternalException(INVALIDARG,"Expected argument 1 to have a __dict__ object, but it does not exist.",t);
    });
    nTypeFuncWrap hasattr = wrapnFunc({
        if (va.size() != 2) throwInternalException(INVALIDNUMOFARGS,"Expected 2 arguments but got " + std::to_string(va.size()),t);
        if (!isofType(va[1],i->strclass)) throwInternalException(INVALIDARG,"Expected argument 2 of type " + i->strclass->name + " but got " + va[1]->base->name,t);
        //ok
        bool isSpecialMethod = (startsWith(((snObj)(va[1]))->sval,"__") && endsWith(((snObj)(va[1]))->sval,"__"));
        string attrname = ((snObj)(va[1]))->sval;
        bool isType = isnType(va[0]);
        __internal_getattr__(va[0],attrname);
        unsigned long long h = va[1]->base->inthash(va[1],t);
        if (va[0]->has__dict__) if (check_key(((vnObj)va[0])->__dict__->dict,h)) return i->truenObj;
        if (isSpecialMethod && isType) {
            if (check_key(((nType)va[0])->funcs,attrname)) return i->truenObj;
        } else if (isSpecialMethod) {
            if (check_key(va[0]->base->funcs,attrname)) return i->truenObj;
        }
        for (nType i : va[0]->base->mro) {
            if (i->has__dict__) {
                unsigned long long h = va[1]->base->inthash(va[1],t);
                if (check_key(((vnObj)i)->__dict__->dict,h)) {
                    return va[0]->i->truenObj;
                }
            }
        }
        return i->falsenObj;
    });
    nTypeFuncWrap dir = wrapnFunc({
        if (va.size() != 1) throwInternalException(INVALIDNUMOFARGS,"Expected 1 argument but got " + std::to_string(va.size()),t);
        vector<string> ret;
        for (nType i : va[0]->base->mro) {
            if (i->has__dict__) {
                for (auto j : ((vnObj)i)->__dict__->dict) {
                    ret.push_back(j.first);
                }
            }
        }
        
    });
};
nTypeFuncWrap& _nType::intgetop(string attr) {
    for (nType i : this->mro) {
        if (check_key(i->funcs,attr)) if (!(i->funcs[attr].isNull())) return i->funcs[attr];
    }
    return this->funcs[attr];
}
_nType::_nType(MainInterpreter i) : _nObj(i) {
    this->base = i->basetypeclass;
}
dictkey& dictkey::createfromstring(string s, MainInterpreter i){
    dictkey t;
    t.key = s;
    snObj so = new _snObj(i);
    so->sval = s;
    t.v = __hash::hashobj(so);
    so->decref();
    return t;
}
dictkey& dictkey::createfromobj(snObj o){
    dictkey t;
    t.key = o->sval;
    t.v = __hash::hashobj(o);
    return t;
}