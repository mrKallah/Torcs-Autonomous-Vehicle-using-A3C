# Torcs Autonomous Vehicle using A3C
AI controller for autonomous cars

## Todo list
- [ ] Tune the parameters for model and tried to make the model consistently learn. 
- [x] [Kallah] Add possibility for c++ to read parameters from .ini file (currently in development, and using .csv)
- [ ] Add more comments for c++
- [ ] Add more comments to python code
- [ ] Clean up the python code (I think some methods are unused and some methods can do with refractoring.)
- [ ] Investigate different model achitectures for the A3C in order to increase performance
- [ ] Detailed instructions for install
- [ ] Detailed instructions for use
- [ ] Detailed instructions for further development
- [ ] [Kallah] Decrease the latency in torcs (we implemented a latency on sending the images, dont remember why)
- [ ] Make the server into a class for more readable code
- [ ] Make the pre-processing into a class for more readable code
- [ ] [Kallah] Remove default torcs files from the folder directory //(I've started this, but its harder than one would think, file x - [ ] and y may have the same content but different dates etc, and so the python script I wrote to automatically get rid of anything that was unedited from the default torcs directory was not good enough as it does not ignore that kind of stuff)
- [ ] Perhaps change to this implementation of A3C: https://github.com/awjuliani/DeepRL-Agents/blob/master/A3C-Doom.ipynb

## Installation
To set up and run the project use the files inside the 'build' folder. More instructions can be found there as well.

## Example video:
### Click to watch video

[![Torcs A3C Demonstration Video](https://images-ext-2.discordapp.net/external/e5VFeD2Dhnfqm6uhzSM2ZDg2GQfwZb_Xk-Fl2mV5B4s/https/i.ytimg.com/vi/d394R58URZI/hqdefault.jpg)](https://www.youtube.com/watch?v=d394R58URZI "Torcs A3C Demonstration Video")

Due to time constraints the parameters tuning and training of model is for the time beeing not optimized. This causes the vehicle to mostly veer to one side, however, though not captured here, it has been able to get though the goal point in two of the test runs. 

This project should be considered as a proof of concept. 

## Credits
original project: https://github.com/dmachlanski/ce903-tesla
