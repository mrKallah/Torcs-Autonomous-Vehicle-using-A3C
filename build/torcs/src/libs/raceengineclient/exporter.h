#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class Exporter {

public:

	Exporter(int width, int height);

	~Exporter();

	unsigned char* resize_img(int& col, int& rew, unsigned char* img);

	void create_save_file(char* path);

	void save_to_file(unsigned char* img);

	void create_client(char * ip, int portnum);

	bool svr_connect();

	void send_msg(unsigned char* img);
	
	void close_connection();

	char* path = "";
	int width, height;
	int client, portnum;
	int bufsize = 1024;
	char * ip;

	//unsigned char *img;
	//unsigned char *img_resize;

	struct sockaddr_in serv_addr;
	struct hostent* server;
};
