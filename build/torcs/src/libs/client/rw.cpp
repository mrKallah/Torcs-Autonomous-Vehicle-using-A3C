#include "rw.h"


void read(string file, string &ip, int &port){
	
	std::ifstream inFile;
    
	int x;
    inFile.open(file);
    if (!inFile) {
        cout << "Unable to open " << file << endl;
        exit(1);
    }
	std::vector<string> text;
    string t;
    while (inFile >> t) {
		text.push_back(t);
	}
    inFile.close();

	ip = text[0];
	port = std::stoi(text[1]);
}

void write(string file, string ip, int port){
	std::ofstream outFile;
	outFile.open (file);
	outFile << ip << " " << port;
	outFile.close();	
}

void str_split(string input, string &variable, string &value){
	string l = "";
	string r = "";
	bool found = false;
	for (int i = 0; i < input.length(); i++){
		if (input[i] == '='){
			found = true;
		} else{
			if (found==true){
				l = l + input[i];
			} else {
				r = r + input[i];
			}
		}
	}
	
	variable = r;
	value = l;
	
}

