//Title: Skeleton Main Window Class
//Platform: BeOS 5
//Version: 2001.12.18
//Description:
//	A class that creates the application's "main" window.
//	This class inherits from BWindow, and runs a message loop for the window.

//Copyright (C) 2001, 2002 Kevin H. Patterson
//Available for use under the terms of the BSD license agreement:
//See the associated file "License.txt" for details.

#ifndef _clsMainWindow_h
#define _clsMainWindow_h

	#include "globals.h"
	#include "TranslatorSavePanel.h"

	

	#include <Application.h>
	#include <View.h>
	#include <Window.h>
	#include <StringView.h>
	#include <TextControl.h>
	#include <Button.h>
	#include <Bitmap.h>
	#include <MenuBar.h>
	#include <MenuItem.h>
	#include <Menu.h>
	#include <CheckBox.h>
	#include <Slider.h>
	#include <app/MessageRunner.h>
	

		// Defining maximal number of layers
	const int MAX_LAYERS=20;


// Conntection between clsMainWindow::layers and DataView::layers
//
//  |---------------------- MAX_LAYERS + 1 -------------------|
//
//   +---------+-----+-----+-----+             +--------------+
//   |    0    |  1  |  2  |  3  |     ....    |  MAX_LAYERS  |   clsMainWindow::layers
//	 +---------+-----+-----+-----+             +--------------+
//
//   +---------+-----+-----+-----+             +--------------+
//   | bgimage |  0  |  1  |  2  |     ....    | MAX_LAYERS-1 |   DataView::layers
//   +---------+-----+-----+-----+             +--------------+
//      ^
//  Background |--------------  MAX_LAYERS -------------------|
//
//   Menu clsMainWindow::layers[0] is a special menu for background,
// which image is stored in "bgimage" variable, not in DataView::layers[0].
//   Therefore menu no. i corresponds to image no. i-1. 
	
	class DrawView : public BView 
	{
	private:
		bool drag_to_tracker;
		bool drag_from_tracker;
		BMessageRunner *messagerunner;
		
	public:
		DrawView(BRect, const char*, uint32, uint32);
		virtual ~DrawView();
		
		void MouseDown(BPoint point);
		void MouseUp(BPoint point);
		
		void DragToTracker(BPoint point);
		void GetTrackerResponse(BMessage *message);
		
		void MessageReceived(BMessage *message);
		
		virtual void Draw(BRect );
		virtual void Draw(void);
		
		static int Rewrite_image (void* );
		
		BBitmap* image;
		BBitmap* bgimage;
		rgb_color bgcolor;
		
		BBitmap* layers[MAX_LAYERS];
//		BView* layers[MAX_LAYERS];		

	friend class clsMainWindow;
	};	
	


	class LayerItem : public BMenu {
		private:
			int num_layer;
			
			BMenuItem* bring_front;
			BMenuItem* bring_fw;
			BMenuItem* send_bw;
			BMenuItem* send_back;
			BMenuItem* del;
			
		public:
		
			LayerItem(const char*, int);
			virtual ~LayerItem();
			
		friend class clsMainWindow;
		friend class DrawView;
	};
	
	
	
	
	class clsMainWindow
	:
		public BWindow
	{
	private:
		bool drag_from_tracker;
			// These functions allow to move layers.
		void Bring_to_front (LayerItem* to_be_first);
		void Send_to_back (LayerItem* to_be_last);
		void Swap_layers (LayerItem* a, LayerItem* b);
		void Swap_layers (int, int);
			// This function compacts the layer arrays
		void compact_layers (void);
			// This function rebuilds the "Layers" menu.
		void Rebuild_Layers (void);
	public:
			// Menus
		BMenuBar* menubar;
		BMenu* M_file;
		BMenu* M_layers;
		BMenu* M_backgr;
		LayerItem* layers[MAX_LAYERS+1];
		BMenuItem* MI_del;
		
			// Text controls
		BTextControl* main_rad;
		BTextControl* sub_rad;
		BTextControl* int_rad;
		BTextControl* hole_pos;
			// Buttons
		BButton* drawB;
		BButton* clearB;
			// Radii
		int	main_R;
		int sub_R;
		int int_R;
		int hole_P;
			// Checkboxes
		BCheckBox* angle;
		BCheckBox* trdg;
			// Slider for pen width
		BSlider* pen_width;
			// maximal angle of drawing, in degrees
		int max_angle;
			// Current GCD (counted once, at "Draw!" press)
		int cur_GCD;
			// Bitmap
		DrawView* Drawing;

		BView* im_bit;
			// Color
		rgb_color Colour;		// Color of the curve
		rgb_color BColour;      // Color of the Background
			// Calculation functions
		void function (double* x, double* y, int, int, int, double deg);
		int GCD (int a, int b);
			// Translator Save Panel
		TranslatorSavePanel* savepanel;
		BFilePanel* openpanel;
			// Drawing thread ID
		long threadID;

		
		static int DrawImage (void *);		
		void SaveImage (BMessage *message);
		void SetImage (BMessage *message);
		clsMainWindow(const char *uWindowTitle);
		~clsMainWindow();
		virtual void MessageReceived(BMessage *Message);
		virtual bool QuitRequested();
	};

#endif
