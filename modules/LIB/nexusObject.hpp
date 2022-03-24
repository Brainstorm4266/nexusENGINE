#pragma once
#define nType _nType*
#define nObj _nObj*
#define nnObj _nnObj*
#define snObj _snObj*
#define inObj _inObj*
#define MainInterpreter _MainInterpreter*
class _nType;
class _nObj;
class Thread;
class _MainInterpreter {
public:
    vector<nObj> objs;
    nType basetypeclass,baseobjclass;
    nObj nilnObj,falsenObj,truenObj;
    vector<Thread*> threads;
};
class Thread {
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
    long long ival = 0;
};
class _snObj : public _nObj {
public:
    string sval;
};
class _nnObj : public _nObj {
public:
    long double val = 0;
};
class vanObj {
public:
    vector<nObj> va;
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
    vector<nObj>& va = v.va; if (true) code\
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
    nTypeFuncWrap 
    
    add,sub,mul,div,pow,
    and,or,xor,inv,
    pos,neg,
    lnot,land,lor,lxor,
    call,hash,
    newobj,initobj,delprepobj,delobj,
    getitem,setitem,length,contains,
    getattr,setattr,hasattr,dir
    
    ;
    _nType(MainInterpreter i);
};
_nType::_nType(MainInterpreter i) : _nObj(i) {
    
}