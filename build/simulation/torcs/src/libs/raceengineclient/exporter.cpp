#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <netdb.h>

#include "exporter.h"


using namespace std;

Exporter::Exporter(unsigned char* img, int width, int height){
	this->img = img;
	this->width = width;
	this->height = height;
	bufsize = width*height*3;
}

Exporter::~Exporter() {}

void Exporter::resize_img(int& col, int& rew) {
	cout << "Resizing....\n";
	img_resize = (unsigned char*)malloc(230400);
	int r, g, b;
	for (int y = 0; y < height; y+=2) {
		for (int x = 0; x < width; x+=2) {
			r = *(img + (width*3*y + x*3));
			g = *(img + (width*3*y + x*3 + 1));
			b = *(img + (width*3*y + x*3 + 2));
			r += *(img + (width*3*y + x*3 + 3));
			g += *(img + (width*3*y + x*3 + 4));
			b += *(img + (width*3*y + x*3 + 5));
			r += *(img + (width*3*(y+1) + x*3));
			g += *(img + (width*3*(y+1) + x*3 + 1));
			b += *(img + (width*3*(y+1) + x*3 + 2));
			r += *(img + (width*3*(y+1) + x*3 + 3));
			g += *(img + (width*3*(y+1) + x*3 + 4));
			b += *(img + (width*3*(y+1) + x*3 + 5));
			r=r/4; g=g/4; b=b/4;
			*(img_resize + ((width*3*y/4) + (x*3/2))) = r;
			*(img_resize + ((width*3*y/4) + (x*3/2))+1) = g;
			*(img_resize + ((width*3*y/4) + (x*3/2))+2) = b;
		}
	}
	//img = img_resize;
	width = 320;
	height = 240;
	bufsize = width*height*3;
	cout << "Resized!\n";
	
	rew ++;
	if (col > 1) {
		col = 1;
	}
		
	*(img_resize + 0) = rew;
	*(img_resize + 1) = col;
	
	cout << "pxl1 = " << *(img_resize + 0) << ", pxl2 = " << *(img_resize + 1) << endl;
	
	
}

void Exporter::create_save_file(char* path) {
	this->path = path;
}

void Exporter::save_to_file() {
	cout << "Saving!\n";
	int pixel;
	std::ofstream outfile;
	outfile.open(path);
	if (outfile.is_open()) {
		for (int i=0; i < width*height*3; i++) {
			pixel = *(img+i);
			if ((i!=0) && ((i%(width*3))==0)) {outfile<<"\n";}
			if (((i+1)%(width*3))==0) {outfile<<pixel;
				} else {outfile<<pixel<<",";}
		}
	} else {
		cout << "<-----FILE OPEN FAILED----->" << endl;	
	}
	outfile.close();
}

void Exporter::create_client(char * ip, int portnum) {
	this->portnum = portnum;
	this->ip = ip;
	client = socket(AF_INET, SOCK_STREAM, 0);
	if (client < 0) {
		cout << "ERROR establishing socket\n" << endl;
		exit(1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portnum);
	inet_pton(AF_INET, ip, &serv_addr.sin_addr);

	cout << "\n--> Socket client created...\n";
}

bool Exporter::svr_connect() {
	server = gethostbyname(ip);
	if (server == NULL) {
		cout << "<-----ERROR: SERVER DOES NOT EXIST----->";
		exit(0);
	} else {
		cout << "->Server Found->\n";
	}
	if (connect(client, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0) {
		cout << "--> Connection to the server " << inet_ntoa(serv_addr.sin_addr)
			<< " with port number: "
			<< portnum << endl;
		return true;
	} else {
		cout << "<-----CONNECTION FAILED----->\n\n";
		return false;
	}
}

void Exporter::send_msg() {
	cout << "->Sending....->\n\n";
	//std::string sName(reinterpret_cast<char*>(name));
	cout << *(img_resize) << endl;
	cout << *(img_resize+1000) << endl;
	send(client, img_resize, bufsize, 0);
	cout << "------>Sent\n\n";
}

void Exporter::close_connection() {
	close(client);
}


