/***************************************************************************

    file                 : ce903.cpp
    created              : Fri Mar 8 18:03:50 GMT 2019
    copyright            : (C) 2002 Rob Mayger

 ***************************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <math.h>
#include <fstream>
#include <cstdlib> 
#include <sstream>
#include <iostream>
#include <string>
#include <thread>

#include <tgf.h> 
#include <track.h> 
#include <car.h> 
#include <raceman.h> 
#include <robottools.h>
#include <robot.h>



#include "ini_reader.h"




using namespace std;

static tTrack	*curTrack;

string const HOME = getenv("HOME") ? getenv("HOME") : ".";
string path = "/tesla";

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s); 
static void newrace(int index, tCarElt* car, tSituation *s); 
static void drive(int index, tCarElt* car, tSituation *s); 
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt); 


string reward_type = "";




/* 
 * Module entry point  
 */ 
extern "C" int 
ce903(tModInfo *modInfo) 
{
    memset(modInfo, 0, 10*sizeof(tModInfo));

    modInfo->name    = strdup("ce903");		/* name of the module (short) */
    modInfo->desc    = strdup("");	/* description of the module (can be long) */
    modInfo->fctInit = InitFuncPt;		/* init function */
    modInfo->gfId    = ROB_IDENT;		/* supported framework version */
    modInfo->index   = 1;

    return 0; 
} 

/* Module interface initialization. */
static int 
InitFuncPt(int index, void *pt) 
{ 
    tRobotItf *itf  = (tRobotItf *)pt; 

    itf->rbNewTrack = initTrack; /* Give the robot the track view called */ 
				 /* for every track change or new race */ 
    itf->rbNewRace  = newrace; 	 /* Start a new race */
    itf->rbDrive    = drive;	 /* Drive during race */
    itf->rbPitCmd   = NULL;
    itf->rbEndRace  = endrace;	 /* End of the current race */
    itf->rbShutdown = shutdown;	 /* Called before the module is unloaded */
    itf->index      = index; 	 /* Index used if multiple interfaces */
    return 0; 
} 

/* Called for every track change or new race. */
static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s) { 
    curTrack = track;
    *carParmHandle = NULL; 
} 

/* 	Anything needed to be called at the start of a new race. 
	This function is ran at the start of each new race, including restarts. 
	Any parameters that needs to be read from ini file goes in here.
*/
static void newrace(int index, tCarElt* car, tSituation *s) { 
    
    string inifile (HOME + path + "/torcs.ini");
	// find_var(filename, category, value name, variable to assign value to)
	find_var(inifile, "torcs", "reward_type", reward_type);
	
	cout << "###############################################" << endl;
	cout << "##### Startinga new race with parameters: " << endl;
	cout << "# Reward type = " << reward_type << endl;
	cout << "###############################################" << endl;
	
    
} 


/*	Drive during race. 
	This method reads the instruction file to know what actions to perform. 
	It then uses those instructions to change the output of the vehicle.
*/
static void drive(int index, tCarElt* car, tSituation *s) { 
    memset((void *)&car->ctrl, 0, sizeof(tCarCtrl)); 
    
    /*  
     * add the driving code here to modify the 
     * car->_steerCmd 
     * car->_accelCmd 
     * car->_brakeCmd 
     * car->_gearCmd 
     * car->_clutchCmd 
     */ 
	ifstream instructions_file (HOME + path + "/drive_instructions.csv");
    string in;
    float accel, steer;
    int restart, gear, clutch, brake;
	
	string inifile (HOME + path + "/instructions.ini");
	// find_var(filename, category, value name, variable to assign value to)
	string tmp = "";
	find_var(inifile, "steer", "accel", tmp);
	accel = stof(tmp);
	
	find_var(inifile, "steer", "steer", tmp);
	steer = stof(tmp);
	
	find_var(inifile, "steer", "restart", tmp);
	restart = stoi(tmp);
	
	find_var(inifile, "steer", "gear", tmp);
	gear = stoi(tmp);
	
	find_var(inifile, "steer", "clutch", tmp);
	clutch = stoi(tmp);
	
	find_var(inifile, "steer", "brake", tmp);
	brake = stoi(tmp);
	
	 /*  
     * add the driving code here to modify the 
     * car->_steerCmd 
     * car->_accelCmd 
     * car->_brakeCmd 
     * car->_gearCmd 
     * car->_clutchCmd 
     */ 
	
	
    
    if (reward_type == "center"){
        car->_reward = abs(car->_trkPos.toMiddle / 4) * 100;
    } else {
        car->_reward = car->_distRaced + 95; //+95 as the vehicle starts 95m behind the goal line
    }
    
    
	// sets the appropriate vehicle commands which have previously been read from the instructions
	car->_askRestart = (restart == 1);	
	car->_steerCmd = steer;
    car->_accelCmd = accel;
	car->_clutchCmd = clutch;
    car->_brakeCmd = brake;
    car->_gearCmd = gear;
	
	// getting the thread id
	std::stringstream ss;
	ss << std::this_thread::get_id();
	string thread_id = ss.str();
	
	//system variable manipulation
	// using the thread id to get ports and ip from environment variable 
	string variable = "a" + thread_id + "a";
	
	// convert string to c string
	char var[variable.length() + 1];
	strcpy(var, variable.c_str());
	
	// get environment variable
	const char* sysvar = getenv(var);
	string varsys = "";
	
	// quit if environment variable is empty
	if (sysvar == NULL) {
		cout << "Environment variable sysvar is NULL" << endl;
		sysvar = "NULL";
		exit(-1);
	}

	
    //here the status of the vehicle and program is printed. 
    cout	<< "collision = " << car->_collision 
			<< "\n" << "Reward = " << car->_reward
			<< "\n" << "Restart = " << car->_askRestart
			<< "\n" << "Steer = " << car->_steerCmd
			<< "\n" << "Speed = " << car->_accelCmd
			<< "\n" << "Clutch = " << car->_clutchCmd
			<< "\n" << "Brake = " << car->_brakeCmd
			<< "\n" << "Gear = " << car->_gearCmd 
			<< "\n" << "Thread ID = " << std::this_thread::get_id()
			<< "\n" << "Sysvar = " << sysvar
			<< endl;
	cout << "###################################" << endl;
	
	// checks if 
	if (car->_collision != 0){
		car->_collision = 1;
	}
}

/* End of the current race */
static void
endrace(int index, tCarElt *car, tSituation *s)
{
}

/* Called before the module is unloaded */
static void
shutdown(int index)
{
}

