#pragma once
#include <vector>
#include <string>
#include <set>



using namespace std;

namespace Tokenizer {
    vector<pair<string, int>> tokenize(char* Text);

    void printTokens(vector<pair<string, int>> lexed);
}