#pragma once

#include <vector>
#include <exception>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <tuple>

#define EventType(...) void (*) (__VA_ARGS__)
#define EventTypeW(name,...) void (*name) (__VA_ARGS__)
template <typename... Args>
class Event;
template <class Cls, typename ret, typename... Args>
class InstancedStaticFunctionCaller__static {
    ret (*func)(Cls*,Args...) = nullptr;
public:
    InstancedStaticFunctionCaller__static(ret (*func)(Cls*,Args...)) : func(func) {}
    ret operator() (Cls* inst, Args... args) {
        return func(inst,args...);
    }
};
template <class Cls, typename ret, typename... Args>
class InstancedStaticFunctionCaller__nonstatic {
    ret (*func)(Cls*,Args...) = nullptr;
    Cls* inst = nullptr;
public:
    InstancedStaticFunctionCaller__nonstatic(ret (*func)(Cls*,Args...),Cls* inst) : func(func), inst(inst) {}
    ret operator() (Args... args) {
        return func(inst,args...);
    }
};
template <typename... Args>
using EventCaller = InstancedStaticFunctionCaller__nonstatic<Event<Args...>,void,Args...>;
struct ____EVENTHANDLER_PRIVATE {
    void* handle;
    void* func;
    void* inst;
}; // very name
template <typename... Args>
class EventHandler final {
    EventTypeW(handle,Args...);
    InstancedStaticFunctionCaller__nonstatic<void,void,EventType(Args...)> func;
public:
    EventHandler(____EVENTHANDLER_PRIVATE e) {
        this->handle = e.handle;
        this->func = InstancedStaticFunctionCaller__nonstatic<void,void,EventType(Args...)>(e.func,e.inst);
        
    }
    void Disconnect() {
        func(handle);
    }
};
template<typename... Args>
class Event final {
    mutable std::vector<EventType(Args...)> handles;
    static void fh(Event<Args...>* e,EventTypeW(f,Args...), Args... args) {
        f(args...);
    }
    static void InternalCall(Event<Args...>* e,Args... args) {
        e->lastcall = std::tuple<Args...>(args...);
        e->cv.notify_all();
        for (auto& handle : e->handles) {
            std::thread(fh,e,handle,args...).detach();
        }
    }
    bool isDisconnected(EventTypeW(f,Args...)) { // searches in handles vector for function
        return handles.end() == std::find(handles.begin(),handles.end(),f);
    }
    static void Disconnect(Event<Args...>* e,EventTypeW(f,Args...)) {
        if (!e->isDisconnected(f)) {
            e->handles.erase(std::find(e->handles.begin(),e->handles.end(),f));
        }
    }
    mutable bool callermade = false;
    mutable std::mutex m;
    mutable std::condition_variable cv;
    mutable std::tuple<Args...> lastcall;
public:
    ____EVENTHANDLER_PRIVATE Connect(EventTypeW(f,Args...)) {
        ____EVENTHANDLER_PRIVATE e;
        e.handle = f;
        e.func = Disconnect;
        e.inst = this;
        handles.push_back(f);
        return e;
    };
    std::tuple<Args...> Wait() {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk);
        return lastcall;
    }
    InstancedStaticFunctionCaller__nonstatic<Event<Args...>,void,Args...> operator() () {
        if (callermade) throw std::runtime_error("Caller already made.");
        callermade = true;
        return EventCaller<Args...>(this->InternalCall,this);
    };
    Event<Args...>& operator=(const Event<Args...>& e) {
        this->handles = e.handles;
        return *this;
    };
    Event(const Event<Args...>& e) {
        this->handles = e.handles;
    }
    Event() {}
};