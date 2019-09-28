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

Exporter::Exporter(int width, int height){
	this->width = width;
	this->height = height;
	bufsize = width*height*3;
}

Exporter::~Exporter() {

}

/*	This method reshapes the image form an unknown shape (needs further looking into) to a
	rgb image where each of the different channels contains only one color. 
	This method also encodes the reward and collition state into the image for sending. 
*/
unsigned char* Exporter::reshape(int& col, int& rew, unsigned char* img) {
	//cout << "Resizing....\n";
	
	unsigned char* img_resize = (unsigned char*)malloc(230400);
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
		
	*(img_resize + 0) = rew;
	*(img_resize + 1) = col;
	free(img);
	return img_resize;
}


void Exporter::create_client(char * ip, int portnum) {
	this->portnum = portnum;
	this->ip = ip;
	client = socket(AF_INET, SOCK_STREAM, 0);
	if (client < 0) {
		cout << "ERROR establishing socket\n" << endl;
		exit(-1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portnum);
	inet_pton(AF_INET, ip, &serv_addr.sin_addr);

	//cout << "\n--> Socket client created...\n";
}

// Connects the client to the python server.
bool Exporter::svr_connect() {
	server = gethostbyname(ip);
	if (server == NULL) {
		cout << "<-----ERROR: SERVER DOES NOT EXIST----->";
		exit(-1);
	} else {
		//cout << "->Server Found->\n";
	}
	if (connect(client, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0) {
		//cout << "--> Connection to the server " << inet_ntoa(serv_addr.sin_addr)
		//	<< " with port number: "
		//	<< portnum << endl;
		return true;
	} else {
		//cout << "<-----CONNECTION FAILED----->\n\n";
		return false;
	}
}

// sends the image to the python server.
void Exporter::send_msg(unsigned char* img) {
	send(client, img, bufsize, 0);
}
// closes connection
void Exporter::close_connection() {
	close(client);
}


