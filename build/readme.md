# How to use the simulator

Firstly use the install instructions in the install file to correctly install the program. 

The program is divided into two seperate programs. A server and a simulator.

## Server

To start a server, open a Terminal window and use

```shell
cd ~/tesla
python3 A3C-classification.py
```

This will now wait for the simulator to start

## Simulator

To start the simulator, open a new Terminal window and use

```shell
torcs
```

Start a race in Quickrace mode. 

The server and client will now connect and the training process has started

## files (this is not extensive)
### src/drivers/ce903/ce903.cpp
This file is where one can control the driving vehicle itself. This is where the instructions are read to control the vehicle and the parameters from the ini file. Any command for the vehicles controll such as turning angle, break, gear, acceleration, etc needs to be given here. Any information about the car such as distance to walls, distance from midlle, from start, if the vehicle has collided are all extracted from this class.
Restarting is also done from this class.

### src/libs/raceengineclient/raceengine.cpp 
This is the main engine for racing. Only some (maybe one) method(s) within this file has been changed. 
The primary method that has been changed is the ReUpdate method. In this method the image is extracted from the video buffer and extracted as a unsigned char array. This array is then fed to the exporter class which is the server class for this project. 

### src/libs/raceengineclient/exporter.cpp
This is the server that handles the sending of the image from the c++ environment to the python environment. This also contains a reshaping method (incorrectly named as resize_img). This reshapes the image from one shape to another. Its unclear as to what the original shape was however the new shape is a rgb image where each channel contains the pixel values for one color. 

### src/drivers/ce903/ini_reader.cpp
This is the functions that handles the reading of the ini files. This might not be the most secure way of handeling it, but it avoided having to install more c++ dependencies, making for a more difficult install. If security is of high importance this file should be looked at. 

### src/interfaces/car.h
This is where global car variables are handeled. If you want to be able to see a value for a car in the race engine, you would have to define a variable 'xyz' in car.h and in the ce903.cpp use car->xyz=123 and in the raceengine.cpp use s->cars[0]->xyz to read that variables value. 

### src/libs/client/mainmenu.cpp
This is the splash screen of the torcs game. This is where the ports are handled for the c++ side of things. When a client is opened, it reads the ~/tesla/ports.txt and then increments the port in that file by one. This ensures the next client starts with a different port. If the ports are busy, the ports file can be changed to the lower number of a range of ports. 

### src/libs/client/rw.cpp
This program handles the writing and reading from and to the ~/tesla/port.txt file for the main menu. 
