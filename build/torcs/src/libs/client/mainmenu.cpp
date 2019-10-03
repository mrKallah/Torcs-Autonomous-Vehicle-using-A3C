/***************************************************************************

    file                 : mainmenu.cpp
    created              : Sat Mar 18 23:42:38 CET 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: mainmenu.cpp,v 1.4.2.1 2008/08/16 23:59:54 berniw Exp $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <stdio.h>
#include <tgfclient.h>
#include <singleplayer.h>
#include <driverconfig.h>
#include <thread>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <sstream> 


#include "mainmenu.h"
#include "exitmenu.h"
#include "optionmenu.h"
#include "rw.h"


string const HOME = getenv("HOME") ? getenv("HOME") : ".";
void *menuHandle = NULL;
tModList *RacemanModLoaded = (tModList*)NULL;

static void
TorcsMainMenuActivate(void * /* dummy */)
{
	if (RacemanModLoaded != NULL) {
		GfModUnloadList(&RacemanModLoaded);
	}
}




/*
 * Function
 *	TorcsMainMenuInit
 *
 * Description
 *	init the main menus
 *
 * Parameters
 *	none
 *
 * Return
 *	0 ok -1 nok
 *
 * Remarks
 *	
 */
int TorcsMainMenuInit(void) {
    menuHandle = GfuiScreenCreateEx((float*)NULL, 
				    NULL, TorcsMainMenuActivate, 
				    NULL, (tfuiCallback)NULL, 
				    1);

    GfuiScreenAddBgImg(menuHandle, "data/img/splash-main.png");
	
	GfuiTitleCreate(menuHandle, "TORCS", 0);

    GfuiLabelCreate(menuHandle, "The Open Racing Car Simulator", GFUI_FONT_LARGE, 320, 420, GFUI_ALIGN_HC_VB, 0);

    GfuiMenuButtonCreate(menuHandle, "Race", "Races Menu", ReSinglePlayerInit(menuHandle), GfuiScreenActivate);
	
    GfuiMenuButtonCreate(menuHandle, "Configure Players", "Players configuration menu", TorcsDriverMenuInit(menuHandle), GfuiScreenActivate);

    GfuiMenuButtonCreate(menuHandle, "Options", "Configure", TorcsOptionOptionInit(menuHandle), GfuiScreenActivate);
    
    GfuiMenuDefaultKeysAdd(menuHandle);

    GfuiMenuBackQuitButtonCreate(menuHandle, "Quit", "Quit TORCS", TorcsMainExitMenuInit(menuHandle), GfuiScreenActivate);

	
	
	
	
	
	string file = HOME + "/tesla/ports.txt";
	
	string ip;
	int port;
	read(file, ip, port);
	write(file, ip, port+1);
	
	std::stringstream ss;
	ss << std::this_thread::get_id();
	string thread_id = ss.str();
	
	//system variable manipulation
	string _str = "a" + thread_id + "a=" + ip + ":" + std::to_string(port);
	
	string variable = "a" + thread_id + "a";
	string value = ip + ":" + std::to_string(port);
	
	//str_split(_str, variable, value);
	
	char var[variable.length() + 1];
	strcpy(var, variable.c_str());
	
	char val[value.length() + 1];
	strcpy(val, value.c_str());
	
	setenv(var, val, 1);

	
	cout << "####### end ############" << endl;
	
    return 0;
}

/*
 * Function
 *	
 *
 * Description
 *	
 *
 * Parameters
 *	
 *
 * Return
 *	
 *
 * Remarks
 *	
 */
int
TorcsMainMenuRun(void)
{
    GfuiScreenActivate(menuHandle);
    return 0;
}
