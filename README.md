# Torcs Autonomous Vehicle using A3C
AI controller for autonomous cars

[![Torcs A3C Demonstration Video](https://images-ext-2.discordapp.net/external/e5VFeD2Dhnfqm6uhzSM2ZDg2GQfwZb_Xk-Fl2mV5B4s/https/i.ytimg.com/vi/d394R58URZI/hqdefault.jpg)](https://www.youtube.com/watch?v=d394R58URZI "Torcs A3C Demonstration Video")

Due to time constraints the parameters tuning and training of model is for the time beeing not optimized. This causes the vehicle to mostly veer to one side, however, though not captured here, it has been able to get though the goal point in two of the test runs. 

This project should be considered as a proof of concept. 

## Todo list
- [ ] Tune the parameters for model and tried to make the model consistently learn. 
- [x] [Kallah] Add possibility for c++ to read parameters from .ini file 
- [ ] Add more comments for c++.
  - [x] [Kallah] ce903.cpp/h
  - [x] [Kallah] exporter.cpp/h
  - [x] [Kallah] raceengine.cpp/h
  - [x] [Kallah] inireader.cpp/h
  - [x] [Kallah] car.h
- [ ] Add more comments to python code
  - [ ] classify.py
  - [ ] helper.py
  - [ ] model.py
  - [ ] pro.py (consider renaming)
  - [ ] server.py
  - [ ] shared_adam.py (dont know what this is, investigate if needed and rename according to what it does if needed)
  - [ ] util(s).py (add the utils and util scripts together if possible, refractoring needed)
  - [ ] video_frame.py
- [ ] Clean up the python code (I think some methods are unused and some methods can do with refractoring.)
- [ ] Investigate different model achitectures for the A3C in order to increase performance
- [ ] Detailed instructions for install
- [ ] Detailed instructions for use
- [ ] Detailed instructions for further development
- [x] [Kallah] Decrease the latency in torcs (we implemented a latency on sending the images, dont remember why)
- [ ] Make the server into a class for more readable code
- [ ] Make the pre-processing into a class for more readable code
- [ ] [Kallah] Remove default torcs files from the folder directory //(I've started this, but its harder than one would think, file x and y may have the same content but different dates etc, and so the python script I wrote to automatically get rid of anything that was unedited from the default torcs directory was not good enough as it does not ignore that kind of stuff)
- [ ] Perhaps change to this implementation of A3C: https://github.com/awjuliani/DeepRL-Agents/blob/master/A3C-Doom.ipynb
- [ ] Change to regression rather than classification for steering, maybe add accel too. (should be controlled by .ini param)
- [ ] Allow for change of gears and add option to ini parameter
- [ ] Allow for change of gears to be controlled by A3C, on/off by parameters
- [x] [Kallah] Solve memory issue in torcs exporter class. SOLUTION: memory leak due to bad memalloc() with non-sane free() methodology. 
- [ ] allow for multiple torcs clients running by having a port number file increment, I.E. client 1 reads port 200, writes port 201 which the next client uses and so on. 
- [ ] look into restart memory leak which is apparently inherrent to TORCS itself. each reload uses approx 100-200mb of ram
- [ ] make the pre-processing image into a boolean image. This could help the model avoid local optima, would be similar to semantic segmentation as seen in https://arxiv.org/pdf/1801.05299.pdf, however without the sementics as onyl the road is maintained in the image fed to the model. 
- [x] [Kallah] change the driver instructions to be a ini file rather than a csv file for more readable code, avoid duplicate code and to make the driver instructions more readable. 
- [x] [Kallah] rename the resize_img function in the exporter class to reshape_img, as it reshapes, it does not resize. 
- [x] [Kallah] find and get rid of unused methods in the exporter class. 

## Installation
To set up and run the project use the files inside the 'build' folder. More instructions can be found there as well.

## Credits
original project: https://github.com/dmachlanski/ce903-tesla
