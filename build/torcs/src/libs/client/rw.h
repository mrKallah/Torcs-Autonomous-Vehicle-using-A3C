#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::string;

void read(string file, string &ip, int &port);

void write(string file, string ip, int port);

void str_split(string input, string &variable, string &value);