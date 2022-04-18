#include <vector>

#pragma once

using std::vector;
template <typename T>
class _GCHead {
public:
    const size_t size = sizeof(T);
    long long refcnt = 0;
    T data = {0};
};
template <typename T>
class Object {
    _GCHead<T>* data = nullptr;
public:
    Object() : data(nullptr) {}
    Object(T o) {
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
    Object& operator=(const Object& o) {
        if (data != nullptr) {
            data->refcnt--;
            if (data->refcnt == 0) {
                delete data;
            }
        }
        data = o.data;
        data->refcnt++;
        return *this;
    }
    Object& operator=(const T& o) {
        if (data != nullptr) {
            data->refcnt--;
            if (data->refcnt == 0) {
                delete data;
            }
        }
        data->data = o;
        data->refcnt = 1;
        return *this;
    }
    Object(const Object& o) {
        data = o.data;
        data->refcnt++;
    }
    ~Object() {
        if (data != nullptr) {
            data->refcnt--;
            if (data->refcnt == 0) {
                delete data;
            }
        }
    }
};
