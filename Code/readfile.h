#pragma once
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool ReadFile(const char* pFileName, string& outFile);
string ReadFile(const char* pFileName);