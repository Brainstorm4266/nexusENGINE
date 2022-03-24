#pragma once
#include <vector>
#include <string>
#include "nexusObject.hpp"
using namespace std;
enum CodeInstruction {
    CLEAR_STACK,
    LOAD_CONST,
    LOAD_NIL,
    LOAD_FALSE,
    LOAD_TRUE,
    POP_LAST
};
enum ConstArgType {
    STRING,
    NUMBER,
    BOOL,
    INTEGER,
    NONE
};
struct CodeArg {
    ConstArgType type;
    union {
        bool b;
        long double f;
        long long i;
        string s;
    };
};
struct CodeObject {
    vector<CodeInstruction> code;
    vector<vector<CodeArg>> args;
    vector<string> stringConsts;
    vector<long double> numberConsts;
    vector<long long> intConsts;
};
class Thread;
nObj runCode(Thread* t, CodeObject& c) { /*
Notice: You MUST return nil at the end of code if there is a chance it wont do a return statement.
This is not handled by this function.
*/
    vector<nObj> stack;
    bool running = true;
    unsigned long long i = 0;
    while (running) {
        if (i >= c.code.size()) return nilnObj;
        switch(c.code[i]) {
            case CLEAR_STACK:
                while (stack.size() != 0) {
                    stack[stack.size() - 1]->decref();
                    stack.pop_back();
                }
                break;
            case LOAD_CONST:
                if (true) { 
                    nObj o;
                    CodeArg& ca = c.args[i][0];
                    switch (ca.type) {
                        case STRING:
                            
                    }
                }
        }
    }
    return nilnObj;
}