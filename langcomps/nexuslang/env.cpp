#include "interpreter.h"

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
vector<vector<pair<int, unsigned char*>>> print(vector<vector<pair<int, unsigned char*>>> args) {
	cout << args[0][0].second << endl;
	return vector<vector<pair<int, unsigned char*>>>();
}

vector<vector<pair<int, unsigned char*>>> warn(vector<vector<pair<int, unsigned char*>>> args) {
	printf("%c[%dm",0x1B, 33);
	cout << args[0][0].second << endl;
	printf("%c[%dm", 0x1B, 0);
	return vector<vector<pair<int, unsigned char*>>>();
}
// Types: 0x0 Not Found, 0x1 CFunc, 0x2 Func, 0x3 Table, 0x4 Reference, 0x5 String
// Data headers: If type is 0x1, set only 1st data array to function pointer.
// If type is 0x3, set only 1st data array to data pointers. This can be updated.

int Interpreter::createObject(int _type, string _name) {
	int _id = objects.size();
	nexuslangObject _OBJ = nexuslangObject(_id, _type, _name);
	objects.push_back(_OBJ);
	return _id;
}

int ENV;

void Interpreter::setUpEnv() {
	int _ENV = createObject(0x3, "_ENV");
	ENV = _ENV;
	int _warn = createObject(0x1, "warn");
	objects[_ENV].data.push_back(vector<unsigned char*>(
			{ reinterpret_cast<unsigned char*>(objects[_warn].id) }
		)
	);
	int warnRefId = funcRefs.size();
	funcRefs.push_back(warn);
	funcRefNames.push_back(pair<string, int>("warn", warnRefId));
	objects[_warn].data.push_back(vector<unsigned char*>(
			{ reinterpret_cast<unsigned char*>(warnRefId) }
		)
	);
	int _print = createObject(0x1, "print");
	objects[_ENV].data.push_back(vector<unsigned char*>(
			{ reinterpret_cast<unsigned char*>(objects[_print].id) }
		)
	);
	int printRefId = funcRefs.size();
	funcRefs.push_back(print);
	funcRefNames.push_back(pair<string,int>("print", printRefId));
	objects[_print].data.push_back(vector<unsigned char*>(
			{ reinterpret_cast<unsigned char*>(printRefId) }
		)
	);

}

int Interpreter::getFunctionIdFromName(string name) {
	int id = -1;
	for (int i = 0; i < funcRefNames.size(); i++) {
		if (funcRefNames[i].first == name) {
			id = funcRefNames[i].second-1;
		}
	}
	return id;
}

int Interpreter::findObject(string name) {
	for (int i = 0; i < objects.size(); i++) {
		nexuslangObject _OBJ = objects[i];
		if (_OBJ.name == name) {
			return i;
		}
	}
	return -1;
}

int Interpreter::findEnvObject(string name) {
	for (int i = 0; i < objects[ENV].data.size(); i++) {
		int v = (int)objects[ENV].data[i][0];
		if (objects[v].name == name) {
			return v;
		}
		else {
			continue;
		}
	}
	return -1;
}