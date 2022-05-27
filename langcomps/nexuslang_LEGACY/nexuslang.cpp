#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <set>
#include "tokenizer.hpp" // lexy from emkay? (used to be "lexer.h")
#include "interpreter.hpp"

using namespace std;
using namespace Tokenizer;

int main(int argc, char *argv[]) {
    //function{\n  for i,v in getfenv() {\n        print(i,v)\n    }\n}
    string text = "5*9"; // FIX TOKENIZER ISSUE!     me from later: what was this issue again?
    // accept argument -e to enter text from stdin
    if (argc > 1 && string(argv[1]) == "-e") {
        stringstream ss;
        string line;
        while (getline(cin, line)) {
            if (line == "<EOF>") {
                // remove last newline
                text = text.substr(0, text.size() - 1);
                break;
            }
            ss << line << endl;
        }
        text = ss.str();
    }
    std::cout << "nexuslang v0.0.0\nCopyright 2022 Andrei Dmitriev\n\n[TKN]: Tokenizing code located in binary... ";
    char* text_char = const_cast<char*>(text.c_str());
    vector<pair<string, int>> LEXED_CONTENT = tokenize(text_char);
    std::cout << "Done!" << std::endl;
    printTokens(LEXED_CONTENT);
    Interpreter* intep = new Interpreter(LEXED_CONTENT);
    intep->setUpEnv();
    if (intep->checkSyntax().second) {
        intep->run();
    }
    else {
        cout << "Syntax error." << endl;
    }
}