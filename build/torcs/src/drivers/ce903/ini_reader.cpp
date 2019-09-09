#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
using namespace std;

string makelower(const string & s)
{
    string str = s;
    for( string::iterator i = str.begin(); i != str.end(); ++i)
        if (tolower(*i) != (int)*i)
            *i = (char)tolower(*i);
    return str;
}

string clear_spaces(string &s){
	string out;
	
	for (int i = 0; i < s.length(); i++){
		if (s[i] != ' '){
			out = out + s[i];
		}
	}
	return out;
}

string clear_comments(string &s){
	string out;
	
	string _s = s;
	_s[_s.length()-1] = ';';
	
	for (int i = 0; i < _s.length(); i++){
		if (_s[i] != ';'){
			out = out + _s[i];
		} else {
			i = _s.length()+1;
		}
	}
	return out;
}

string clean(string &s){
	s = clear_comments(s);
	s = clear_spaces(s);
	s = makelower(s);
	return s;
}

void split(string s, string &a, string &b){
	bool found = false;
	bool just_found = false;
	
	for (int i = 0; i < s.length(); i++){
		if (s[i] == '='){
			found = true;
			just_found = true;
		}
		
		if (!found) {
			a = a + s[i];
		} else {
			if (!just_found) {
				b = b + s[i];
			} else {
				just_found = false;
			}
		}
	}
	
}

vector <vector<string>> readConfigFile(string & inifile){
	ifstream file( inifile.c_str() );
    if (!file){
		cout << "File not found" << "\n";
		exit(-1);
	}
	
	vector <string> types;
	vector <string> variables;
	vector <string> values;
	
	string s;
	string type;

	while (getline(file, s)) {
		bool found_open  = false;
		bool found_close  = false;
		for (int i = 0; i < s.length(); i++){
			if (s[i] == '['){
				found_open = true;
			} else if (s[i] == ']'){
				found_close = true;
			}
		}
		if (found_open && found_close){
			type = clean(s);
		} else if (s == "\r"){
			;
		} else if (s == "\n"){
			;
		} else if (s == "\r\n"){
			;
		} else if (s == "\n\r"){
			;
		} else if (s == " "){
			;
		} else if (s == ""){
			;
		} else {
			s = clean(s);
			string variable = "";
			string value = "";
			
			split(s, variable, value);
			
			types.push_back(type);
			variables.push_back(variable);
			values.push_back(value);
			
		}
    }
    file.close();
	vector <vector<string>> vectors;
	vectors.push_back(types);
	vectors.push_back(variables);
	vectors.push_back(values);
	return vectors;
}

void find_var(string inifile, string type, string variable, string &value){
	type = "[" + type + "]";
	
	vector <vector<string>> vectors;
	vectors = readConfigFile(inifile);
	
	vector <string> types = vectors[0];
	vector <string> variables = vectors[1];
	vector <string> values = vectors[2];
	
	for (int i = 0; i < types.size(); i++){
		//cout << types[i] << " " << variables[i] << " = " << values[i] << endl;	
		if (types[i].compare(type) == 0){
			if (variables[i].compare(variable) == 0){
				value = values[i];
			}
		}
	}
}