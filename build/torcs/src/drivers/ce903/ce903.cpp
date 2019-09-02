/***************************************************************************

    file                 : ce903.cpp
    created              : Fri Mar 8 18:03:50 GMT 2019
    copyright            : (C) 2002 Rob Mayger

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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

#include <tgf.h> 
#include <track.h> 
#include <car.h> 
#include <raceman.h> 
#include <robottools.h>
#include <robot.h>


using namespace std;

static tTrack	*curTrack;

string const HOME = getenv("HOME") ? getenv("HOME") : ".";

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s); 
static void newrace(int index, tCarElt* car, tSituation *s); 
static void drive(int index, tCarElt* car, tSituation *s); 
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt); 


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
static void  
initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s) 
{ 
    curTrack = track;
    *carParmHandle = NULL; 
} 

/* Start a new race. */
static void  
newrace(int index, tCarElt* car, tSituation *s) 
{ 
} 

bool has_col = false;
/* Drive during race. */
static void  
drive(int index, tCarElt* car, tSituation *s) 
{ 
    memset((void *)&car->ctrl, 0, sizeof(tCarCtrl)); 
    
    /*  
     * add the driving code here to modify the 
     * car->_steerCmd 
     * car->_accelCmd 
     * car->_brakeCmd 
     * car->_gearCmd 
     * car->_clutchCmd 
     */ 
	
	
	ifstream instructions_file (HOME + "/tesla/drive_instructions.csv");
    string in;
    bool no_instructions = true;
    float accel, steer;
    int restart;
    if (instructions_file.is_open()) {
        try{
			getline(instructions_file, in, ',');
            accel = stof(in); 
        } catch(const exception& e) {
         cout << "in = " << in << endl;
        }
        try{
			getline(instructions_file, in, ',');
			steer = stof(in); 
        } catch (const exception& e) {
         cout << "in = " << in << endl;
        }
        
        try{
			getline(instructions_file, in, ','); 
			restart = stoi(in);
        } catch (const exception& e) {
            cout << "in = " << in << endl;
        }
    } else {
        cout << "----Could not open Driver Instructions----\n";
    }
    //cout << "Distance from start: " << car->_distRaced + 100 << endl;
    //cout << "Distance from middle: " << car->_trkPos.toMiddle << endl;
    //cout << "Collision: " << car->_collision << endl;
    float reward = 0;
    if (car->_trkPos.toMiddle < 1 && car->_trkPos.toMiddle > -1) {
		//reward += 1
        //reward += 2;
		reward = 2;
		
    } else {
        //reward -= 1;
        //reward += 1;
		reward = 1;
		
		
    }
    car->_reward = reward;
    //cout << "Reward = " << reward << endl;
    car->_brakeCmd = 0;
    car->_gearCmd = 1;
    //car->_clutchCmd 
	car->_accelCmd = accel;
	car->_steerCmd = steer;
    car->_askRestart = (restart == 1);	
	
	if (car->_collision != 0){
		has_col = true;
	}
	
	if (has_col == true) {
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

