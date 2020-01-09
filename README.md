# Torcs Autonomous Vehicle using A3C
AI controller for autonomous cars

[![Torcs A3C Demonstration Video](https://img.youtube.com/vi/p0b_gyu-4Js/maxresdefault.jpg)](https://youtu.be/p0b_gyu-4Js "Torcs A3C Demonstration Video")

The image above is a link to a video that show the latest example of the development. 

## Todo list
### C++
- [x] [Kallah] Add possibility for c++ to read parameters from .ini file 
- [x] [Kallah] Add more comments for c++.
  - [x] [Kallah] ce903.cpp/h
  - [x] [Kallah] exporter.cpp/h
  - [x] [Kallah] raceengine.cpp/h
  - [x] [Kallah] inireader.cpp/h
  - [x] [Kallah] car.h
- [x] [Kallah] Decrease the latency in torcs (we implemented a latency on sending the images, dont remember why)
- [ ] [Kallah] Remove default torcs files from the github //(I've started this, but its harder than one would think, file x and y may have the same content but different dates etc, and so the python script I wrote to automatically get rid of anything that was unedited from the default torcs directory was not good enough as it does not ignore that kind of stuff).
- [x] [Kallah] Allow for change of gears, clutch and brakes and add option to ini parameter
- [x] [Kallah] Solve memory issue in torcs exporter class. SOLUTION: memory leak due to bad memalloc() with non-sane free() methodology. 
- [x] [Kallah] Allow for multiple torcs clients running by having a port number file increment, I.E. client 1 reads port 200, writes port 201 which the next client uses and so on. 
- [x] [Kallah] look into restart memory leak which is apparently inherrent to TORCS itself. each reload uses approx 100-200mb of ram
- [x] [Kallah] change the driver instructions to be a ini file rather than a csv file for more readable code, avoid duplicate code and to make the driver instructions more readable. 
- [x] [Kallah] rename the resize_img function in the exporter class to reshape_img, as it reshapes, it does not resize. 
- [x] [Kallah] find and get rid of unused methods in the exporter class. 
- [ ] Remove using namespace std in c++ files to remove bad C++ practices. 
- [x] [Kallah] move making images grayscale into c++ for increased performance. 
- [x] [Kallah] move preprocessing steps into c++

### Python
- [ ] Tune the parameters for model and tried to make the model consistently learn. 
- [x] Add more comments to python code
- [x] [Kallah] Clean up the python code (I think some methods are unused and some methods can do with refractoring.)
- [ ] Investigate different model achitectures for the A3C in order to increase performance
- [x] [Kallah] Remake the communication protocol
- [x] [Kallah] {DEPRICATED: MOVED TO C++} Make the pre-processing into a class for more readable code
- [ ] Perhaps change to this implementation of A3C: https://github.com/awjuliani/DeepRL-Agents/blob/master/A3C-Doom.ipynb
- [ ] Change to regression rather than classification for steering, maybe add accel too. (should be controlled by .ini param)
- [ ] Allow for change of gears, clutch and brakes to be controlled by A3C, on/off by parameters
- [ ] make the pre-processing image into a boolean image. This could help the model avoid local optima, would be similar to semantic segmentation as seen in https://arxiv.org/pdf/1801.05299.pdf, however without the sementics as onyl the road is maintained in the image fed to the model. 
- [x] [Kallah] Move to grayscale images for faster image pre-processing
- [x] [Kallah] Rework preprocessing to increase performance

### Other
- [ ] Detailed instructions for install
- [ ] Detailed instructions for use
- [ ] Detailed instructions for further development

## Installation
To set up and run the project use the files inside the 'build' folder. More instructions can be found there as well.

## Credits
original project: https://github.com/dmachlanski/ce903-tesla
