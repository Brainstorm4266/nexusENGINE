#pragma once
#include <vector>
#include <string>
#include "nexusObject.hpp"
using namespace std;
enum CodeInstruction {
    CLEAR_STACK,
    LOAD_CONST,
    POP_LAST,
    BEGIN_CALLSTACK,
    END_CALLSTACK_AND_CALL,
    TRY_CODEBLOCK,
    CATCH_CODEBLOCK,
    FINALLY_CODEBLOCK
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
};
nObj runCode(Thread* t, CodeObject& c) { /*
Notice: You MUST return nil at the end of code if there is a chance it wont do a return statement.
*/
    vector<nObj> stack;
    bool running = true;
    unsigned long long i = 0;
    while (running) {
        if (i >= c.code.size()) return t->i->nilnObj;
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
                            o = new _snObj(t->i);
                            ((_snObj*)o)->sval = ca.s;
                            stack.push_back(o);
                        case NUMBER:
                            o = new _nnObj(t->i);
                            ((_nnObj*)o)->nval = ca.f;
                            stack.push_back(o);
                        case BOOL:
                            if (ca.b) {
                                o = t->i->truenObj;
                                o->incref();
                                stack.push_back(o);
                            } else {
                                o = t->i->falsenObj;
                                o->incref();
                                stack.push_back(o);
                            }
                        case INTEGER:
                            o = new _inObj(t->i);
                            ((_inObj*)o)->ival = ca.i;
                            stack.push_back(o);
                        case NONE:
                            o = t->i->nilnObj;
                            o->incref();
                            stack.push_back(o);
                    }
                }
            case POP_LAST:
                if (stack.size() != 0) {
                    stack[stack.size() - 1]->decref();
                    stack.pop_back();
                }
            
        }
    }
    return t->i->nilnObj;
}