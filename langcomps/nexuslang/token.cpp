#include "interpreter.hpp"

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

string Interpreter::getNextToken(bool completeOnEOF) {
	string token;
	bool gotToken = false;
	int pager = currentToken;
	while (!gotToken) {
		if (!isInString) {
			pair<string, int> tkn;
			tkn = lexedContent[static_cast<__int64>(pager) + 1];
			if ((tkn.second == 0) && (tkn.first == "<EOF>") && (completeOnEOF)) {
				isCompleted = true;
				break;
			}
			if ((tkn.second == 2) && (tkn.first == "<SPACE>")) {
				pager++;
				continue;
			} else if ((tkn.second == 2) && (tkn.first == "<TAB>")) {
				pager++;
				continue;
			}
			else {
				token = tkn.first;
				gotToken = true;
				break;
			}
		}
		else {
			pair<string, int> tkn = lexedContent[static_cast<__int64>(pager) + 1];
			token = tkn.first;
			gotToken = true;
			break;
		}
	}
	return token;
}

string Interpreter::getCurrentToken(bool completeOnEOF) {
	string token;
	bool gotToken = false;
	int pager = currentToken;
	while (!gotToken) {
		if (!isInString) {
			pair<string, int> tkn;
			tkn = lexedContent[static_cast<__int64>(pager)];
			if ((tkn.second == 0) && (tkn.first == "<EOF>") && (completeOnEOF)) {
				isCompleted = true;
				break;
			}
			if ((tkn.second == 2) && (tkn.first == "<SPACE>")) {
				pager++;
				continue;
			}
			else if ((tkn.second == 2) && (tkn.first == "<TAB>")) {
				pager++;
				continue;
			}
			else {
				token = tkn.first;
				gotToken = true;
				break;
			}
		}
		else {
			pair<string, int> tkn = lexedContent[static_cast<__int64>(pager)];
			token = tkn.first;
			gotToken = true;
			break;
		}
	}
	return token;
}

string Interpreter::getLastToken(bool completeOnEOF) {
	string token;
	bool gotToken = false;
	int pager = currentToken;
	while (!gotToken) {
		if (!isInString) {
			pair<string, int> tkn;
			tkn = lexedContent[static_cast<__int64>(pager) - 1];
			if ((tkn.second == 0) && (tkn.first == "<EOF>") && (completeOnEOF)) {
				isCompleted = true;
				break;
			}
			if ((tkn.second == 2) && (tkn.first == "<SPACE>")) {
				pager = pager - 1;
				continue;
			}
			else if ((tkn.second == 2) && (tkn.first == "<TAB>")) {
				pager = pager - 1;
				continue;
			}
			else {
				token = tkn.first;
				gotToken = true;
				break;
			}
		}
		else {
			pair<string, int> tkn = lexedContent[static_cast<__int64>(pager) - 1];
			token = tkn.first;
			gotToken = true;
			break;
		}
	}
	return token;
}

pair<string, int> Interpreter::getToken(int token) {
	return lexedContent[token];
}

void Interpreter::switchToken(int tokenId) {
	pair<string, int> tkn = lexedContent[tokenId];
	if ((tkn.second == 0) && (tkn.first == "<EOF>")) {
		isCompleted = true;
	}
	else {
		currentToken = tokenId;
	}
}