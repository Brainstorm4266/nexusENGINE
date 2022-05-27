#include "./tokenizer.hpp"
#include "./utils.hpp"

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

using namespace std;

bool eraseSubStr(std::string& mainStr, const std::string& toErase)
{
    // Search for the substring in string
    size_t pos = mainStr.find(toErase);
    if (pos != std::string::npos)
    {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
        return true;
    }
    return false;
}

// Let's set some stuff up. Int 0 = Hidden token (non-visible characters like \n and <EOF>), 1 = Token, 2 = Special
vector<pair<string, int>> Tokenizer::tokenize(char* Text) {
    vector<pair<string, int>> lexed;
    string SText = string(Text);
    string CURRENT_TOKEN = "";
    //cout << "\n";
    for (int i = 0; i < SText.length()+1; i++) {
        const char Char = SText[i];
        bool IS_SPECIAL_TOKEN = false;
        //cout << CURRENT_TOKEN << endl;
        //(Char != ' ') && 
        if ((Char != ' ') && (Char != '\n')) {
            CURRENT_TOKEN = CURRENT_TOKEN + Char;
        }
        switch (Char) {
            case '\0': {
                //cout << "End reached, flushing token." << endl;
                //IS_SPECIAL_TOKEN = true;
                if (!lexed.empty()) {
                    //if ((!CURRENT_TOKEN.size() == 0) && (lexed.back().first != CURRENT_TOKEN)) {
                    //    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    //    CURRENT_TOKEN = "";
                    //}
                }
                else {
                    if (!CURRENT_TOKEN.size() == 0) {
                        lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                        CURRENT_TOKEN = "";
                    }
                }
                break;
            }
            case '\n': {
                //cout << "NEWLINE, CURRENT TOKEN: " << CURRENT_TOKEN << endl;
                IS_SPECIAL_TOKEN = true;
                if (!lexed.empty()) {
                    if ((!CURRENT_TOKEN.size() == 0) && (lexed.back().first != CURRENT_TOKEN)) {
                        lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                        CURRENT_TOKEN = "";
                    }
                }
                else {
                    if (!CURRENT_TOKEN.size() == 0) {
                        lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                        CURRENT_TOKEN = "";
                    }
                }
                lexed.push_back(pair<const char*, int>("newline", 0));
                break;
            }
            case ' ': {
                //cout << CURRENT_TOKEN.size() << endl;
                IS_SPECIAL_TOKEN = true;
                if (!CURRENT_TOKEN.size() == 0) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<string, int>("<SPACE>", 2));
                break;
            }
            case '  ': {
                //cout << CURRENT_TOKEN.size() << endl;
                IS_SPECIAL_TOKEN = true;
                if (!CURRENT_TOKEN.size() == 0) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<string, int>("<TAB>", 2));
                break;
            }
            case ',': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, ",");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != ",")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>(",", 2));
                break;
            }
            case '"': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "\"");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "\"")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("\"", 2));
                break;
            }
            case '\'': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "\'");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "'")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("\'", 2));
                break;
            }
            case '{': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "{");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "{")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("{", 2));
                break;
            }
            case '}': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "}");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "}")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("}", 2));
                break;
            }
            case '(': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "(");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "(")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("(", 2));
                break;
            }
            case ')': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, ")");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != ")")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>(")", 2));
                break;
            }
            case '+': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "+");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "+")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("+", 2));
                break;
            }
            case '-': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "-");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "-")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("-", 2));
                break;
            }
            case '*': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "*");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "*")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("*", 2));
                break;
            }
            case '/': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "/");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "/")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("/", 2));
                break;
            }
            case '%': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, "%");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != "%")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>("%", 2));
                break;
            }
            case ';': {
                IS_SPECIAL_TOKEN = true;
                eraseSubStr(CURRENT_TOKEN, ";");
                if ((!CURRENT_TOKEN.size() == 0) && (CURRENT_TOKEN != ";")) {
                    lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                    CURRENT_TOKEN = "";
                }
                lexed.push_back(pair<const char*, int>(";", 2));
                break;
            }
        }
        if ((Char == '\0')&&(!IS_SPECIAL_TOKEN)) {
            if (CURRENT_TOKEN[0] != '\0') {
                lexed.push_back(pair<string, int>(CURRENT_TOKEN, 1));
                CURRENT_TOKEN = "";
            }
        }
    }
    lexed.push_back(pair<string, int>("<EOF>", 0));
    return lexed;
}