#pragma once
#define nType _nType*
#define nObj _nObj*
class _nType;
class _nObj {
public:
    nType base = nullptr;
    unsigned long long refcnt = 1;
    void incref() {
        this->refcnt++;
    }
    void decref() {
        this->refcnt--;
    }
};
nObj nilnObj;
nObj falsenObj;
nObj truenObj;