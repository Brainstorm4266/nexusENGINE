#pragma once
#include <vector>
#include <string>

using namespace std;

class nexuslangObject {
public:
	int id;
	int type;
	string name;
	vector<vector<unsigned char*>> data;
	nexuslangObject(int _id, int _type, string _name) {
		id = _id;
		type = _type;
		name = _name;
		data = vector<vector<unsigned char*>>();
	}
	//int _funcRef;
	//vector<int> _members;
};

class Interpreter {
public:
	// System functions
	vector<nexuslangObject> objects;
	vector<vector<nexuslangObject>> CFuncArgs;
	vector<vector<nexuslangObject>> CFuncReturns;
	vector<vector<vector<pair<int, unsigned char*>>>(*)(vector<vector<pair<int, unsigned char*>>> args)> funcRefs;
	vector<vector<pair<int, unsigned char*>>> tempargs;
	vector<string> strings;
	vector<pair<string, int>> funcRefNames;
	int getFunctionIdFromName(string);
	string parseString();
	int createObject(int _type, string _name);
	void run();
	int findObject(string name);
	int findEnvObject(string name);
	string getNextToken(bool completeOnEOF = true);
	string getCurrentToken(bool completeOnEOF = true);
	string getLastToken(bool completeOnEOF = true);
	int parseMath();
	pair<string, int> getToken(int token);
	pair<string, bool> checkSyntax();
	void setUpEnv();
	// TODO: Make _ENV.
	// TODO: Make Abstract Syntax Tree (AST) and Action Tree (AT) compiler
	Interpreter(vector<pair<string, int>> lexed);
	bool isInString;
	int currentToken;
	void switchToken(int tokenId);
	bool isCompleted;
	vector<pair<string, int>> lexedContent;
};