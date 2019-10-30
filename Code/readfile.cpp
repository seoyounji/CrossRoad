#include "readfile.h"


bool ReadFile(const char* pFileName, string& outFile)
{
	ifstream f(pFileName);

	bool ret = false;

	if (f.is_open()) {
		string line;
		while (getline(f, line)) {
			outFile.append(line);
			outFile.append("\n");
		}

		f.close();

		ret = true;
	}
	else {
		ret = false;
	}

	return ret;
}

string ReadFile(const char* pFileName)
{
	string outFile;

	ifstream f(pFileName);

	if (!f.is_open()) return "";

	string line;
	while (getline(f, line)) {
		outFile.append(line);
		outFile.append("\n");
	}
	return outFile;
}
