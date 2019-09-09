#ifndef INI_READER_H
#define INI_READER_H



#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
using namespace std;



string makelower(const string & s);
string clear_spaces(string &s);
string clear_comments(string &s);
string clean(string &s);
void split(string s, string &a, string &b);
vector <vector<string>> readConfigFile(string & inifile);
void find_var(string inifile, string type, string variable, string &value);



#endif