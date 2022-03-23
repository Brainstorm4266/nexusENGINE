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
#include <cstring>

using namespace std;

Interpreter::Interpreter(vector<pair<string, int>> lexed) {
	isInString = false;
	lexedContent = lexed;
}

pair<string, bool> Interpreter::checkSyntax() {
	return pair<string, bool>("No errors.", true);
}
bool stringSuccess = false;
string Interpreter::parseString() {
	string str = "";
	if (getCurrentToken() == "\"") {
		switchToken(currentToken + 1);
		while (true) {
			if (isCompleted) {
				std::cout << "Unexpected end of file before string terminator." << endl;
				stringSuccess = false;
				break;
			}
			if ((lexedContent[currentToken].second == 0) && (lexedContent[currentToken].first == "newline")) {
				std::cout << "Unexpected newline before string terminator." << endl;
				stringSuccess = false;
				break;
			}
			if (getCurrentToken() == "\"") {
				switchToken(currentToken + 1);
				stringSuccess = true;
				break;
			}
			if (lexedContent[currentToken].second == 2) {
				if (lexedContent[currentToken].first == "<SPACE>") {
					str = str + " ";
				}
				else {
					str = str + getCurrentToken();
				}
			}
			else {
				str = str + getCurrentToken();
			}
			switchToken(currentToken + 1);
		}
	}
	return str;
}
bool mathSuccess = false;
int Interpreter::parseMath() {
	mathSuccess = true;
	try {
		cout << "Last token: " << getLastToken() << endl;
		cout << "Current token: " << getCurrentToken() << endl;
		cout << "Next token: " << getNextToken() << endl;
		if (getCurrentToken() == "+") {
			return stoi(getLastToken()) + stoi(getNextToken(false));
		}
		else if (getCurrentToken() == "*") {
			return stoi(getLastToken()) * stoi(getNextToken(false));
		}
		else if (getCurrentToken() == "/") {
			return stoi(getLastToken()) / stoi(getNextToken(false));
		}
		else if (getCurrentToken() == "-") {
			return stoi(getLastToken()) - stoi(getNextToken(false));
		}
		else if (getCurrentToken() == "%") {
			return stoi(getLastToken()) % stoi(getNextToken(false));
		}
		else {
			mathSuccess = false;
			return NULL;
		}
	}	
	catch (exception e) {
		mathSuccess = false;
		return NULL;
	}
}

bool errored = false;

void Interpreter::run() {
	vector<pair<string, int>> lexed = lexedContent;
	std::cout << "\n\nnexuslang JIT compiler v0.0.0\n\n[RUNNING]\n";
	std::cout << "[PRINTING COMMAND]\n\n";
	//std::cout << funcRefs[findObject("print").data[0][0]](1) << endl;
	for (int i = 0; i < lexed.size(); i++) {
		pair<string, int> entry = lexed[i];
		if ((entry.second != 0)&&(entry.second != 2)) {
			std::cout << entry.first;
		}
		else if ((entry.first != "<SPACE>") && (entry.first != "newline") && (entry.first != "<TAB>") && (entry.first != "<EOF>")) {
			std::cout << entry.first;
		}
		else if (entry.first == "<SPACE>") {
			std::cout << " ";
		}
		else if (entry.first == "<TAB>") {
			std::cout << "	";
		}
		else if (entry.first == "newline") {
			std::cout << "\n";
		}
		else if (entry.first == "<EOF>") {
			break;
		}
	}

	std::cout << "\n\n[CREATING LOOP FOR EXECUTION]\n\n";
	while (true) {
		if (isCompleted) {
			break;
		}
		bool isFunction = false;
		if ((getCurrentToken() == "+") && (!isInString)) {
			cout << parseMath() << endl;
		}
		if ((getCurrentToken() == "-") && (!isInString)) {
			cout << parseMath() << endl;
		}
		if ((getCurrentToken() == "*") && (!isInString)) {
			cout << parseMath() << endl;
		}
		if ((getCurrentToken() == "/") && (!isInString)) {
			cout << parseMath() << endl;
		}
		if ((getCurrentToken() == "%") && (!isInString)) {
			cout << parseMath() << endl;
		}
		if ((getNextToken() == "(")&&(!isInString)) {
			isFunction = true;
			std::cout << "Token " << getCurrentToken() << " is a function call. Checking if exists..." << endl;
			string funcname = getCurrentToken();
			int argsId;
			vector<vector<pair<int, unsigned char*>>> args = vector<vector<pair<int, unsigned char*>>>();
			if (findEnvObject(getCurrentToken()) == -1) {
				cout << "ERROR: Function \"" << getCurrentToken() << "\" not found." << endl;
				break;
			}
			if (objects[findEnvObject(getCurrentToken())].type == 0x1) {
				argsId = CFuncArgs.size();
				CFuncArgs.push_back(vector<nexuslangObject>());
			}
			else {
				cout << "ERROR: " << getCurrentToken() << " is not a valid CFunction!" << endl;
				break;
			}
			cout << "Function exists." << endl;
			string e;
			char* e_arg;
			while (!(getNextToken() == ")")) {
				//std::cout << getCurrentToken() << endl;
				if (getCurrentToken() == "\"") {
					e = parseString();
					e_arg = const_cast<char*>(e.c_str());
					if (!stringSuccess) {
						errored = true;
						break;
					}
					args.push_back(vector<pair<int, unsigned char*>>
						(
							{
								pair<int, unsigned char*>(0x5, reinterpret_cast<unsigned char*>(e_arg))
							}
						)
					);
					if (!((getCurrentToken() == ")") || (getCurrentToken() == ","))) {
						std::cout << "ERROR: Unexpected token \"" << getCurrentToken() << "\" in argument list.";
						errored = true;
						break;
					}
				}
				//cout << (const char*)args[0][0].second << endl;
				switchToken(currentToken + 1);
				// TODO: add "," (comma) as seperator
				if (isCompleted) {
					break;
				}
				if (getNextToken() == "(") {
					std::cout << "ERROR: Unexpected token: " << getNextToken() << "\n// TODO: make support for prioritized \"run this first\" arguments." << endl;
					errored = true;
					return;
				}
			}
			if (errored) {
				return;
			}
			if (!(getLastToken() == ")")) {
				std::cout << "ERROR: End of file before argument list end!" << endl;
				errored = true;
				return;
			}
			cout << "\n[EXECUTING FUNCTION]" << endl << endl;
			funcRefs[(int)objects[findEnvObject(funcname)].data[0][0]](args);
			cout << "\n[FUNCTION EXECUTION FINISHED]" << endl << endl;
			switchToken(currentToken + 1);
			//std::cout << getCurrentToken() << endl;
		}
		switchToken(currentToken+1);
	}
	std::cout << "\n\n[EXECUTION FINISHED]\n\n";
	std::cout << "\n\n\n";
}

