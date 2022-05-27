#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

enum {
	HIDDEN,
	TOKEN,
	SPECIAL_TOKEN
};

using namespace std;

int len(string str);

void split(string str, char seperator);