#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class Exporter {

public:

	Exporter(int width, int height);

	~Exporter();

	unsigned char* resize(int& col, int& rew, unsigned char* img);

	unsigned char* flip_and_mirror(int height, int width, int colorspace, unsigned char* img);

	void save_img(unsigned char* img);

	std::string int_to_chars(int size, int &bit_count);

	void write_to_fifo(unsigned char* img, int port, int reward, int collision, int height, int width);


	char* path = "";
	int width, height;
	int client, portnum;
	int bufsize = 1024;
	char * ip;

	struct sockaddr_in serv_addr;
	struct hostent* server;
};
