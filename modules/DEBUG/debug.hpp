#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "../LIB/nxeExcHead.hpp"
#include "../LIB/win.hpp"
#include "../LIB/to_string.hpp"

#define EN_CS(name) __debug::callstack.push_back(__debug::CallStackObj{#name,__FILE__,__LINE__});
#define EX_CS() __debug::callstack.pop_back();
#define G_CS __debug::GetCallstack()
#define debug_print(...) __debug::printDbg(__VA_ARGS__);
namespace __debug {
    using std::string;
    using std::stringstream;
    using std::ofstream;
    using std::vector;
    using std::unordered_map;
    using std::wstring;
    using std::ostream;
    struct CallStackObj {
        string name;
        string file;
        int line;
    };
    class {
        unordered_map<unsigned long,vector<CallStackObj>> callstack;
    public:
        void push_back(CallStackObj obj) {
            if (callstack.find(GetCurrentThreadId()) == callstack.end()) {
                callstack[GetCurrentThreadId()] = vector<CallStackObj>();
            }
            callstack[GetCurrentThreadId()].push_back(obj);
        }
        void pop_back() {
            if (callstack.find(GetCurrentThreadId()) == callstack.end()) {
                return;
            }
            callstack[GetCurrentThreadId()].pop_back();
            if (callstack[GetCurrentThreadId()].size() == 0) {
                callstack.erase(GetCurrentThreadId());
            }
        }
        string getCallStackForThread(unsigned long threadid) {
            if (callstack.find(threadid) == callstack.end()) {
                return "";
            }
            stringstream ss;
            ss << "Callstack for thread " << threadid << ":\n";
            for (auto &i : callstack[threadid]) {
                ss << "    " << i.file << " " << i.line << ": " << i.name << "\n";
            }
            return ss.str();
        }
        string GetCallstackForAllThreads() {
            stringstream ss;
            for (auto &i : callstack) {
                ss << getCallStackForThread(i.first);
            }
            return ss.str();
        }
    } callstack;
    string GetCallstack() {
        return callstack.GetCallstackForAllThreads();
    }
    template <typename a>
    void __printvdbg(vector<string>& s, a b) {
        s.push_back(to_string(b));
        s.push_back("\n");
    }
    template <typename a, typename... Args>
    void __printvdbg(vector<string>& s, a b, Args... args) {
        s.push_back(to_string(b));
        s.push_back("\t");
        __printvdbg(s, args...);
    }
    template <typename... Args>
    void printDbg(Args... args) {
        stringstream ss;
        vector<string> s;
        __printvdbg(s, args...);
        for (auto& i : s) {
            ss << i;
        }
        s.push_back("\n");
        string sv = ss.str();
        wstring ws = std::wstring(sv.begin(), sv.end());
        OutputDebugString(ws.c_str());

        cout << ws.c_str();
    }
    template <typename T>
    ostream& operator << (ostream& o,vector<T> v) {
        o << "[";
        for (auto& i : v) {
            o << i << ",";
        }
        o.seekp(-1,o.cur);
        o << "]";
        return o;
    }
    template <typename T>
    stringstream& operator << (stringstream& o,vector<T> v) {
        o << "[";
        for (auto& i : v) {
            o << i << ",";
        }
        o.seekp(-1,o.cur);
        o << "]";
        return o;
    }
}