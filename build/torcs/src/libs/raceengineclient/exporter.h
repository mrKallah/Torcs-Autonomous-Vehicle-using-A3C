#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class Exporter {

public:

	Exporter(unsigned char *img, int width, int height);

	~Exporter();

	void resize_img(int& col, int& rew);

	void create_save_file(char* path);

	void save_to_file();

	void create_client(char * ip, int portnum);

	bool svr_connect();

	void send_msg();
	
	void close_connection();

	char* path = "~/torcs/screen_img.csv";
	int width, height;
	int client, portnum;
	int bufsize = 1024;
	char * ip;

	unsigned char *img;
	unsigned char *img_resize;

	struct sockaddr_in serv_addr;
	struct hostent* server;
};
