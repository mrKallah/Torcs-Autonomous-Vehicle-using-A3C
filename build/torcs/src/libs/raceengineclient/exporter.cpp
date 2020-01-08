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

/*	This method resizes the image from 640x480 to 320x240 and
		then encodes the collision and reward to the first and second
		pixel
*/
unsigned char* Exporter::resize(int& col, int& rew, unsigned char* img) {
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
	free(img);
	return img_resize;
}

unsigned char* Exporter::flip_and_mirror(int height, int width, int colorspace, unsigned char* img) {
	// alocate the memory space for the image
	unsigned char* img_resize = (unsigned char*)malloc(width * height * colorspace);

	int j = 0;
	for (int h = 0; h < height; h++){
		int i = 0;
		for (int w = 0; w < width; w++){
			for (int c = 0; c < colorspace; c++){
					// read each pixel starting from the bottom left moving right and then upwards
					int forwards = (height * width * colorspace) - (h * (width * colorspace)) + i - (width * colorspace);
					//replacing pixles
					*(img_resize + forwards) = *(img + j);

					i ++;
					j++;
			}
		}
	}

	free(img);

	return img_resize;
}

unsigned char* Exporter::pre_process(int height, int width, int colorspace, unsigned char* img) {
	// alocate the memory space for the image

	unsigned char* img_resize = (unsigned char*)malloc(width * height * colorspace);

	int threshold = 50;
	int i = 0;
	for (int w = 0; w < width; w++){
		bool found = false;
		for (int h = 0; h < height; h++){
			int r = 0;
			int g = 0;
			int b = 0;
				for (int c = 0; c < colorspace; c++){
					int index = (height * width * colorspace) - ((h * colorspace * width) + w * colorspace) + c - 3;

					if (c == 0){
						r = *(img + index);
					}
					if (c == 1){
						g = *(img + index);
					}
					if (c == 2){
						b = *(img + index);
						if ((r < threshold && g < threshold && b < threshold) || found) {
							*(img_resize + index - 0) = 255;
							*(img_resize + index - 1) = 255;
							*(img_resize + index - 2) = 255;
							found = true;
						} else {
							*(img_resize + index - 0) = *(img + index - 0);
							*(img_resize + index - 1) = *(img + index - 1);
							*(img_resize + index - 2) = *(img + index - 2);
						}
					}
					i++;
				}
		}
	}
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

void Exporter::write_to_fifo(unsigned char* img, int port, int reward, int collision, int height, int width){
		// for some reason collision starts out being 3, so it gets encoded to 0.

	string line;
	string com_file = "/tmp/is" + to_string(port) + "ready";
	string fifo_file = "/tmp/" + to_string(port) + ".fifo";

	// read the file that dictates if the model is ready to recieve an image
	// this file will contain a 0 if the model is not ready,
	// a 1 if the images should be converted to grayscale
	// and a 2 if the image should be rgb
	ifstream myfile (com_file);
	if (myfile.is_open()) {
		getline (myfile, line);
		myfile.close();
	} else {
		cout << "Unable to open file" << endl;
	}

	if (line == "1" || line == "2"){

		int colorspace;

		if (line == "1") {
			colorspace = 1;
		} else {
			colorspace = 3;
		}


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
		int reward_bits = 0;
		int collision_bits = 0;
		// start bits is the amount of bit variables
		int startbits = 5;

		char ctest;



		string s_h = int_to_chars(height, h_bits);
		string s_w = int_to_chars(width, w_bits);
		string s_c = int_to_chars(colorspace, c_bits);
		string s_reward = int_to_chars(reward, reward_bits);
		string s_collision = int_to_chars(collision, collision_bits);

		ctest = static_cast<char>( h_bits );
		s += ctest;

		ctest = static_cast<char>( w_bits );
		s += ctest;

		ctest = static_cast<char>( c_bits );
		s += ctest;

		ctest = static_cast<char>( reward_bits );
		s += ctest;

		ctest = static_cast<char>( collision_bits );
		s += ctest;

		s += s_h + s_w + s_c + s_reward + s_collision;

		// writing image to fifo file
		int pixel = 0;
		int i = 0;
		for (int h = 0; h < height; h++){
			for (int w = 0; w < width; w++){
				for (int rgb = 0; rgb < 3; rgb++){

					// convert to greyscale if needed
					if (colorspace == 1){
						// add pixels to integer value
						pixel += (int)(*(img + i));

						if (rgb == 2){
							// add the accumilation of the three rgb values
							// to the string and set the pixel value to 0
							char ctest = static_cast<char>( pixel / 3 );
							s += ctest;
							pixel = 0;
						}

					}else {
						char ctest = static_cast<char>( (int)(*(img + i)) );
						s += ctest;
					}

					i++;

				}
			}
		}


		const char* cimg = s.c_str();
		// the size of the string is the height * width * colorspace
		// then you have to add the length of the encoding of the h w and c bits and the 3 bits
		// that contain how many bits belongs to h w and c
		write(fd, cimg, height * width * colorspace + h_bits + w_bits + c_bits + reward_bits + collision_bits + startbits + 1);
		close(fd);

		ofstream myfile (com_file);
		if (myfile.is_open()){
			myfile << "0";
			myfile.close();
		}
		else {
			cout << "Unable to open file" << endl;
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
		cout << "Unable to open file" << endl;
	}
}
