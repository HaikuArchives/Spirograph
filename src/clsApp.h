//Title: Skeleton Application Class
//Platform: BeOS 5
//Version: 2001.12.18
//Description:
//	The application class.
//	This class inherits from BApplication, and runs the application's message loop.

//Copyright (C) 2001, 2002 Kevin H. Patterson
//Available for use under the terms of the BSD license agreement:
//See the associated file "License.txt" for details.

#ifndef _clsApp_h
#define _clsApp_h
	
	#include "globals.h"

	#define ApplicationSignature "application/x-vnd.hitech-Spirograph"
	#define SAVE_IMAGE						'saim'
	#define OPEN_FILE_PANEL					'ofpl'
	#define RUN_FILTER						'runf'
	#define DRAG_TO_TRACKER					'drat'

	#define SAVE_FILE_PANEL					'sfpl'
	#define SAVE_FILE_PANEL_FORMAT			'sfpf'
	#define SAVE_FILE_PANEL_SETTINGS		'sfps'

	#include <Application.h>
	
	#include "clsMainWindow.h"
	
	const uint32 MENU_FILE_CLEAR = 'MFnw'; 
	const uint32 MENU_FILE_OPEN = 'MFop'; 
	const uint32 MENU_FILE_CLOSE = 'MFcl'; 
	const uint32 MENU_FILE_SAVE = 'MFsv'; 
	const uint32 MENU_FILE_SAVEAS = 'MFsa'; 
	const uint32 MENU_FILE_PAGESETUP = 'MFps'; 
	const uint32 MENU_FILE_PRINT = 'MFpr'; 
	const uint32 MENU_FILE_QUIT = 'MFqu'; 
	const uint32 MENU_OPT_HELLO = 'MOhl';
	
	const uint32 STOP_DRAWING = 'Stdr';
	const uint32 DRAW_BUTTON = 'draw';
	const uint32 DRAWING_COMPLETE = 'comp';
	
	const uint32 BRING_TO_FRONT = 'Mbtf';
	const uint32 BRING_FORWARD = 'Mbfw';
	const uint32 SEND_BACKWARD = 'Msbw';
	const uint32 SEND_TO_BACK = 'Mstb';
	const uint32 DELETE_LAYER = 'Mdlt';
	
	#define BG_COLOR_RED 		'Bgrd'
	#define BG_COLOR_GREEN 		'Bggr'
	#define BG_COLOR_BLUE  		'Bgbl'
	#define BG_COLOR_CYAN 		'Bgcy'
	#define BG_COLOR_YELLOW  	'Bgye'
	#define BG_COLOR_PINK  		'Bgpi'
	#define BG_COLOR_GREY  		'Bgge'
	
			// Defining number of layers currently in use
	static int overall_number;
	
	class clsApp
	:
		public BApplication
	{
	public:
		clsApp();
		~clsApp();
		virtual void ReadyToRun();
		virtual void Pulse();
	private:
		clsMainWindow *iMainWindow;
	};
	
#endif
