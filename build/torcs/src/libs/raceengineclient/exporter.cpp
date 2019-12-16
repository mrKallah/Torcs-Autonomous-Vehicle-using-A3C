#include <fstream>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstdio>
#include <netdb.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

	unsigned char* img_resize = (unsigned char*)malloc(width * height * 3);
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

			*(img_resize + ((width*3*y/4) + (x*3/2))) = (unsigned char) r;
			*(img_resize + ((width*3*y/4) + (x*3/2))+1) = (unsigned char) g;
			*(img_resize + ((width*3*y/4) + (x*3/2))+2) = (unsigned char) b;

		}
	}
	width = 320;
	height = 240;
	bufsize = width*height*3;

	*(img_resize + 0) = (unsigned char) rew;
	*(img_resize + 1) = (unsigned char) col;

	free(img);
	return img_resize;
}

string Exporter::int_to_chars(int size, int &bit_count){
	// converts any integer to a string of chars.
	// If the integer is larger than 255 it will break it down to multiple chars and
	// the amount of chars will be saved in the bit_count.

	int integer_cast;
	char c;
	string output = "";
	int i;

	// add a char of value 255 to string for each time the size is divisable by 255
	for (i = 0; i < size / 255; i ++){
		c = static_cast<char>( 255 );
		integer_cast = c;
		output += c;
		bit_count++;
	}

	// add the remaining value from the size to the string
	c = static_cast<char>( size - (255 * i) );
	integer_cast = c;
	output += c;
	bit_count++;

	return output;
}

void Exporter::write_to_fifo(unsigned char* img, int port){

	string line;
	string com_file = "/tmp/is" + to_string(port) + "ready";
	string fifo_file = "/tmp/" + to_string(port) + ".fifo";

	ifstream myfile (com_file);
	if (myfile.is_open()) {
		getline (myfile, line);
		myfile.close();
	} else {
		cout << "Unable to open file";
	}

	if (line == "1"){

		int height = 240; //240
		int width = 320; //320
		int colorspace = 3; //3

		//temp string to store info
		string s = "";

		// fifo file to write to
		//FILE* fp = fopen("/tmp/myfifo", "w");
		//char * myfifo = '/tmp/myfifo';
		mkfifo(fifo_file.c_str(), 0666);
		int fd = open(fifo_file.c_str(), O_WRONLY);

		int h_bits = 0;
		int w_bits = 0;
		int c_bits = 0;
		char ctest;

		string s_h = int_to_chars(height, h_bits);
		string s_w = int_to_chars(width, w_bits);
		string s_c = int_to_chars(colorspace, c_bits);


		ctest = static_cast<char>( h_bits );
		s += ctest;

		ctest = static_cast<char>( w_bits );
		s += ctest;

		ctest = static_cast<char>( c_bits );
		s += ctest;

		s += s_h + s_w + s_c;


		// writing image to pnm format
		int i = 0;
		for (int h = 0; h < height; h++){
			for (int w = 0; w < width; w++){
				for (int rgb = 0; rgb < colorspace; rgb++){

					char ctest = static_cast<char>( (int)(*(img + i)) );
					s += ctest;
					i++;

				}
			}
		}

		const char* cimg = s.c_str();
		// the size of the string is the height * width * colorspace
		// then you have to add the length of the encoding of the h w and c bits and the 3 bits
		// that contain how many bits belongs to h w and c
		write(fd, cimg, height * width * colorspace + h_bits + w_bits + c_bits + 3+ 1);
		close(fd);

		ofstream myfile (com_file);
		if (myfile.is_open()){
			myfile << "0";
			myfile.close();
		}
		else {
			cout << "Unable to open file";
		}

	}
	/**
	// uncomment this for step by step processing
	string waiting = "true";
	while(waiting == "true"){
		cout << "img = (" << &img << "), img[0] = (" << (char)*(img + 0) << "), &img[0] = (" << &(img + 0) << "), waiting = " << waiting << ", ";
		cin >> waiting;
		cout << waiting << endl;
	}
	/**/

}

void Exporter::save_img(unsigned char* img){
	ofstream myfile ("/home/kallah/tesla/test.pnm");
  if (myfile.is_open()){
    myfile << "P3 640 480 255\n";
		int i = 0;
		int j = 0;
		for (int w = 0; w < 640 * 480 * 3 ; w++){
				myfile << (int)(*(img + w));
				if (i == 3) {
					i = 0;
					myfile << "\t";
				} else {
					myfile << " ";
				}
				if (j == 640){
					j = 0;
					myfile << "\n";
				}
				i++;
				j++;
		}
    myfile.close();
		string waiting = "true";
		while(waiting == "true"){
			cin >> waiting;
		cout << waiting << endl;
		}
  }
  else {
		cout << "Unable to open file";
	}
}
