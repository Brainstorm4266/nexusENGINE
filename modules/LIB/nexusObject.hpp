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
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

class _nType;
class _nObj;
class Thread;

enum InternalExceptionTypes {
    UNKNOWN,
    MISSINGFUNC,
    INVALIDNUMOFARGS,
    INVALIDARG
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
    _funcnObj(MainInterpreter i) : _nObj(i) {this->base = i->funcclass;};
    nTypeFuncWrap f;
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
#define unerrfunc(token) wrapnFunc({\
    throwInternalException(MISSINGFUNC,(string)"Object of type " + va[0]->base->name + (string)(" does not support operator " token "."),t)\
})
#define bierrfunc(token) wrapnFunc({\
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
    void redoMRO() {
        this->mro.clear();
        this->mro.push_back(this);
        for (auto i : this->bases) {
            this->mro.insert(this->mro.end(),i->mro.begin(),i->mro.end());
        }
        this->mro.erase(std::unique(this->mro.begin(),this->mro.end()),this->mro.end());
    }
    nTypeFuncWrap 
    
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
    nObj intgetattr(nObj o, string attr, Thread* t) {
        vanObj v;
        v.t = t;
        v.va.push_back(o);
        snObj s = new _snObj(this->i);
        s->sval = attr;
        v.va.push_back(s);
        nObj r = o->base->getattr(v);
        s->decref();
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
namespace IndexedSpecialMethods {
    unsigned long long index(string s) {
        switch (s.length()) {
            case 1: return charlistToNumber(s[0]);
            case 2: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63;
            case 3: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63 + charlistToNumber(s[2]) * 63 * 63;
            case 4: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63 + charlistToNumber(s[2]) * 63 * 63 + charlistToNumber(s[3]) * 63 * 63 * 63;
            case 5: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63 + charlistToNumber(s[2]) * 63 * 63 + charlistToNumber(s[3]) * 63 * 63 * 63 + charlistToNumber(s[4]) * 63 * 63 * 63 * 63;
            case 6: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63 + charlistToNumber(s[2]) * 63 * 63 + charlistToNumber(s[3]) * 63 * 63 * 63 + charlistToNumber(s[4]) * 63 * 63 * 63 * 63 + charlistToNumber(s[5]) * 63 * 63 * 63 * 63 * 63;
            case 7: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63 + charlistToNumber(s[2]) * 63 * 63 + charlistToNumber(s[3]) * 63 * 63 * 63 + charlistToNumber(s[4]) * 63 * 63 * 63 * 63 + charlistToNumber(s[5]) * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[6]) * 63 * 63 * 63 * 63 * 63 * 63;
            case 8: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63 + charlistToNumber(s[2]) * 63 * 63 + charlistToNumber(s[3]) * 63 * 63 * 63 + charlistToNumber(s[4]) * 63 * 63 * 63 * 63 + charlistToNumber(s[5]) * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[6]) * 63 * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[7]) * 63 * 63 * 63 * 63 * 63 * 63 * 63;
            case 9: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63 + charlistToNumber(s[2]) * 63 * 63 + charlistToNumber(s[3]) * 63 * 63 * 63 + charlistToNumber(s[4]) * 63 * 63 * 63 * 63 + charlistToNumber(s[5]) * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[6]) * 63 * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[7]) * 63 * 63 * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[8]) * 63 * 63 * 63 * 63 * 63 * 63 * 63 * 63;
            case 10: return charlistToNumber(s[0]) + charlistToNumber(s[1]) * 63 + charlistToNumber(s[2]) * 63 * 63 + charlistToNumber(s[3]) * 63 * 63 * 63 + charlistToNumber(s[4]) * 63 * 63 * 63 * 63 + charlistToNumber(s[5]) * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[6]) * 63 * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[7]) * 63 * 63 * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[8]) * 63 * 63 * 63 * 63 * 63 * 63 * 63 * 63 + charlistToNumber(s[9]) * 63 * 63 * 63 * 63 * 63 * 63 * 63 * 63 * 63;
            default: return -1;
        }
    }
    unsigned long long add = index("add");
    unsigned long long sub = index("sub");
    unsigned long long mul = index("mul");
    unsigned long long div = index("div");
    unsigned long long pow = index("pow");
    unsigned long long mod = index("mod");
    unsigned long long iadd = index("iadd");
    unsigned long long isub = index("isub");
    unsigned long long imul = index("imul");
    unsigned long long idiv = index("idiv");
    unsigned long long ipow = index("ipow");
    unsigned long long imod = index("imod");
    unsigned long long andf = index("and");
    unsigned long long orf = index("or");
    unsigned long long xorf = index("xor");
    unsigned long long inv = index("inv");
    unsigned long long shl = index("shl");
    unsigned long long shr = index("shr");
    unsigned long long iand = index("iand");
    unsigned long long ior = index("ior");
    unsigned long long ixor = index("ixor");
    unsigned long long ishl = index("ishl");
    unsigned long long ishr = index("ishr");
    unsigned long long pos = index("pos");
    unsigned long long neg = index("neg");
    unsigned long long lnot = index("lnot");
    unsigned long long land = index("land");
    unsigned long long lor = index("lor");
    unsigned long long lxor = index("lxor");
    unsigned long long iland = index("iland");
    unsigned long long ilor = index("ilor");
    unsigned long long ilxor = index("ilxor");
    unsigned long long call = index("call");
    unsigned long long hash = index("hash");
    unsigned long long repr = index("repr");
    unsigned long long toint = index("toint");
    unsigned long long tonum = index("tonum");
    unsigned long long tostr = index("tostr");
    unsigned long long newobj = index("newobj");
    unsigned long long initobj = index("initobj");
    unsigned long long delprepobj = index("delprepobj");
    unsigned long long delobj = index("delobj");
    unsigned long long delattr = index("delattr");
    unsigned long long getattr = index("getattr");
    unsigned long long setattr = index("setattr");
    unsigned long long getitem = index("getitem");
    unsigned long long setitem = index("setitem");
    unsigned long long iter = index("iter");
    unsigned long long next = index("next");
    unsigned long long length = index("length");
    unsigned long long delitem = index("delitem");
    unsigned long long neq = index("neq");
    unsigned long long eq = index("eq");
    unsigned long long lt = index("lt");
    unsigned long long gt = index("gt");
    unsigned long long le = index("le");
    unsigned long long ge = index("ge");
    unsigned long long ilt = index("ilt");
    unsigned long long igt = index("igt");
    unsigned long long ile = index("ile");
    unsigned long long ige = index("ige");

};
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
    nTypeFuncWrap andf = bierrfunc("and");
    nTypeFuncWrap orf = bierrfunc("or");
    nTypeFuncWrap xorf = bierrfunc("xor");
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
        if (va.size() < 1) throwInternalException(INVALIDNUMOFARGS,"Expected 1 arguments but got 0",t);
        if (!(va[0]->base == i->basetypeclass || isInVector(va[0]->base->bases,i->basetypeclass))) throwInternalException(INVALIDARG,"Expected argument 1 of type " + i->basetypeclass->name + " but got " + va[0]->base->name,t);
        nObj o = new _nObj(i);
        o->base = (nType)va[0];
        va[0]->incref();
        return o;
    });
    nTypeFuncWrap initobj = wrapnFunc({});
    nTypeFuncWrap delpreobj = wrapnFunc({});
    nTypeFuncWrap delobj = wrapnFunc({delete va[0];});
    nTypeFuncWrap getitem = unerrfunc("getitem");
    nTypeFuncWrap setitem = unerrfunc("setitem");
    nTypeFuncWrap delitem = unerrfunc("delitem");
    nTypeFuncWrap length = unerrfunc("length");
    nTypeFuncWrap contains = unerrfunc("contains");
    #error getattr missing, also the other base object functions
#define __rungetattrfunc_internal_(obj,name) if (true) {\
    using namespace IndexedSpecialMethods;\
    switch (index(name)){\
        case add:if (!obj->add.isNull()) return obj->add; break;\
        case sub:if (!obj->sub.isNull()) return obj->sub; break;\
        case mul:if (!obj->mul.isNull()) return obj->mul; break;\
        case div:if (!obj->div.isNull()) return obj->div; break;\
        case pow:if (!obj->pow.isNull()) return obj->pow; break;\
        case mod:if (!obj->mod.isNull()) return obj->mod; break;\
        case iadd:if (!obj->iadd.isNull()) return obj->iadd; break;\
        case isub:if (!obj->isub.isNull()) return obj->isub; break;\
        case imul:if (!obj->imul.isNull()) return obj->imul; break;\
        case idiv:if (!obj->idiv.isNull()) return obj->idiv; break;\
        case ipow:if (!obj->ipow.isNull()) return obj->ipow; break;\
        case imod:if (!obj->imod.isNull()) return obj->imod; break;\
        case andf:if (!obj->andf.isNull()) return obj->andf; break;\
        case orf:if (!obj->orf.isNull()) return obj->orf; break;\
        case xorf:if (!obj->xorf.isNull()) return obj->xorf; break;\
        case inv:if (!obj->inv.isNull()) return obj->inv; break;\
        case shl:if (!obj->shl.isNull()) return obj->shl; break;\
        case shr:if (!obj->shr.isNull()) return obj->shr; break;\
        case iand:if (!obj->iand.isNull()) return obj->iand; break;\
        case ior:if (!obj->ior.isNull()) return obj->ior; break;\
        case ixor:if (!obj->ixor.isNull()) return obj->ixor; break;\
        case ishl:if (!obj->ishl.isNull()) return obj->ishl; break;\
        case ishr:if (!obj->ishr.isNull()) return obj->ishr; break;\
        case pos:if (!obj->pos.isNull()) return obj->pos; break;\
        case neg:if (!obj->neg.isNull()) return obj->neg; break;\
        case lnot:if (!obj->lnot.isNull()) return obj->lnot; break;\
        case land:if (!obj->land.isNull()) return obj->land; break;\
        case lor:if (!obj->lor.isNull()) return obj->lor; break;\
        case lxor:if (!obj->lxor.isNull()) return obj->lxor; break;\
        case iland:if (!obj->iland.isNull()) return obj->iland; break;\
        case ilor:if (!obj->ilor.isNull()) return obj->ilor; break;\
        case ilxor:if (!obj->ilxor.isNull()) return obj->ilxor; break;\
        case call:if (!obj->call.isNull()) return obj->call; break;\
        case hash:if (!obj->hash.isNull()) return obj->hash; break;\
        case repr:if (!obj->repr.isNull()) return obj->repr; break;\
        case toint:if (!obj->toint.isNull()) return obj->toint; break;\
        case tonum:if (!obj->tonum.isNull()) return obj->tonum; break;\
        case tostr:if (!obj->tostr.isNull()) return obj->tostr; break;\
        case newobj:if (!obj->newobj.isNull()) return obj->newobj; break;\
        case initobj:if (!obj->initobj.isNull()) return obj->initobj; break;\
        case delpreobj:if (!obj->delpreobj.isNull()) return obj->delpreobj; break;\
        case delobj:if (!obj->delobj.isNull()) return obj->delobj; break;\
        case delattr:if (!obj->delattr.isNull()) return obj->delattr; break;\
        case getattr:if (!obj->getattr.isNull()) return obj->getattr; break;\
        case setattr:if (!obj->setattr.isNull()) return obj->setattr; break;\
        case getitem:if (!obj->getitem.isNull()) return obj->getitem; break;\
        case setitem:if (!obj->setitem.isNull()) return obj->setitem; break;\
        case delitem:if (!obj->delitem.isNull()) return obj->delitem; break;\
        case length:if (!obj->length.isNull()) return obj->length; break;\
        case contains:if (!obj->contains.isNull()) return obj->contains; break;\
        case iter:if (!obj->iter.isNull()) return obj->iter; break;\
        case next:if (!obj->next.isNull()) return obj->next; break;\
        case neq:if (!obj->neq.isNull()) return obj->neq; break;\
        case eq:if (!obj->eq.isNull()) return obj->eq; break;\
        case lt:if (!obj->lt.isNull()) return obj->lt; break;\
        case le:if (!obj->le.isNull()) return obj->le; break;\
        case gt:if (!obj->gt.isNull()) return obj->gt; break;\
        case ge:if (!obj->ge.isNull()) return obj->ge; break;\
        case ilt:if (!obj->ilt.isNull()) return obj->ilt; break;\
        case ile:if (!obj->ile.isNull()) return obj->ile; break;\
        case igt:if (!obj->igt.isNull()) return obj->igt; break;\
        case ige:if (!obj->ige.isNull()) return obj->ige; break;\
        default:break;\
    }\
}
    nTypeFuncWrap getattr = wrapnFunc({
        if (va.size() != 2) throwInternalException(INVALIDNUMOFARGS,"Expected 2 arguments but got " + std::to_string(va.size()),t);
        if (!(va[1]->base == i->strclass || isInVector(va[1]->base->bases,i->strclass))) throwInternalException(INVALIDARG,"Expected argument 2 of type " + i->strclass->name + " but got " + va[1]->base->name,t);
        //ok
        bool isSpecialMethod = (startsWith(((snObj)(va[1]))->sval,"__") && endsWith(((snObj)(va[1]))->sval,"__"));
        string attrname = ((snObj)(va[1]))->sval;
        if (isSpecialMethod) attrname = removeLeadingWhitespace(removeLeadingWhitespace(attrname,"__"),"__");
        bool isType = (va[0]->base == i->basetypeclass || isInVector(va[0]->base->bases,i->basetypeclass));
        if (isSpecialMethod && isType) {

        }
    });
};
_nType::_nType(MainInterpreter i) : _nObj(i) {
    this->base = i->basetypeclass;
}