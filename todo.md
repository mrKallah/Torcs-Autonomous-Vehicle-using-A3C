Tune the parameters for model and tried to make the model consistently learn. 
Add possibility for c++ to read parameters from .ini file (currently in development, and using .csv)
Add more comments for c++
Add more comments to python code
Clean up the python code (I think some methods are unused and some methods can do with refractoring.)
Investigate different model achitectures for the A3C in order to increase performance
Detailed instructions for install
Detailed instructions for use
Detailed instructions for further development
Decrease the latency in torcs (we implemented a latency on sending the images, dont remember why)
Make the server into a class for more readable code
Make the pre-processing into a class for more readable code
Remove default torcs files from the folder directory //(I've started this, but its harder than one would think, file x and y may have the same content but different dates etc, and so the python script I wrote to automatically get rid of anything that was unedited from the default torcs directory was not good enough as it does not ignore that kind of stuff)
