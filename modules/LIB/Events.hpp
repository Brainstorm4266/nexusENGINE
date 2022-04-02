#pragma once

#include <vector>

#include <thread>

#define EventType(...) void (*) (__VA_ARGS__)
#define EventTypeW(name,...) void (*name) (__VA_ARGS__)
template <typename... Args>
class Event;
template <typename... Args>
class EventHandler {
    friend class Event;
    EventType(Args) handle;
    EventHandler(EventType(Args) handle, Event* ev) : handle(handle), ev(ev) {}
    Event* ev;
public:
    void Disconnect();
};
template <typename... Args>
class EventCaller {
public:
    void operator() (Args...);
private:
    friend class Event;
    Event ev;
};
template <typename... Args>
class Event {
    std::vector<EventHandler<Args...>> handles;
    friend class EventHandler;
    friend class EventCaller;
    void InternalCall(Args... args) {
        for (auto& handle : handles) {
            std::thread(handle.handle,args...).detach();
        }
    }
    bool callermade;
public:
    EventHandler<Args...>& Connect(EventType(Args...) handle) {
        handles.push_back(EventHandler<Args...>(handle,this));
        return handles.back()
    }
    Event() {
        callermade = false;
    }
    EventCaller<Args...> operator() () {
        if (!callermade) {
            callermade = true;
            return EventCaller<Args...>(*this);
        }
    }
};
template <typename... Args>
void EventCaller<Args...>::operator() (Args... args) {
    ev->InternalCall(args...);
}
template <typename... Args>
void EventHandler<Args...>::Disconnect() {
    ev->handles.erase(std::find(handles.begin(),handles.end(),*this));
}