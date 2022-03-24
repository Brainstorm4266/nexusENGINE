#include "./tokenizer.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

void Tokenizer::printTokens(vector<pair<string, int>> lexed) {
    std::vector<pair<string, int>>::iterator it = lexed.begin();
    std::cout << std::endl << "[   [TOKENIZER DUMP STARTED]   ]" << std::endl;
    while (it != lexed.end())
    {
        // Print the element
        if ((*it).second == 0) {
            std::cout << "[" << (*it).first << ", " << "NON_VISIBLE]" << std::endl;
        }
        else if ((*it).second == 1) {
            std::cout << "[" << (*it).first << ", " << "TOKEN]" << std::endl;
        }
        else if ((*it).second == 2) {
            std::cout << "[" << (*it).first << ", " << "SPECIAL_TOKEN]" << std::endl;
        }
        else {
            std::cout << "[" << (*it).first << ", " << "UNKNOWN]" << std::endl;
        }
        //Increment the iterator
        it++;
    }
    std::cout << "[   [ TOKENIZER DUMP ENDED ]   ]" << std::endl;
}