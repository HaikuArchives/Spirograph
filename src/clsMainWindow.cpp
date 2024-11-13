//Title: Skeleton Main Window Class
//Platform: BeOS 5
//Version: 2001.12.18
//Description:
//	A class that creates the application's "main" window.
//	This class inherits from BWindow, and runs a message loop for the window.

//Copyright (C) 2001, 2002 Kevin H. Patterson
//Available for use under the terms of the BSD license agreement:
//See the associated file "License.txt" for details.

	// This application's headers
#include "clsMainWindow.h"
#include "clsApp.h"
	// General Be headers
#include <Box.h>
#include <RadioButton.h>
#include <StringView.h>
#include <Entry.h>
#include <Bitmap.h>
#include <TextControl.h>
#include <Alert.h>
#include <FilePanel.h>
#include <storage/File.h>
#include <storage/Entry.h>
#include <storage/Path.h>
#include <storage/Directory.h>
#include <storage/Node.h>
#include <storage/NodeInfo.h>
#include <be/interface/GraphicsDefs.h>
#include <be/translation/TranslatorRoster.h>
#include <be/translation/TranslationUtils.h>
#include <be/translation/BitmapStream.h>
#include <be/translation/TranslationDefs.h>
#include <be/translation/TranslatorFormats.h>
#include <be/kernel/OS.h>
	// POSIX headers (most of which are unnecessary)
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


clsMainWindow::clsMainWindow(const char *uWindowTitle)
:
	BWindow(
		BRect(100, 100, 700, 630),
		"Spirograph v1.5",
		B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE )
{
	BView* MainView(
		new BView(
			BWindow::Bounds(), 
			NULL, 
			B_FOLLOW_TOP | B_FOLLOW_LEFT, 
			0)
		);
	this->UpdateIfNeeded();
	if (MainView != NULL)
	{
		MainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		MainView->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		char* text_to_add;
		main_R = 250;
		sub_R = 225;
		int_R = 200;
		hole_P = 100;
		int i=0;
		rgb_color temp_color;
		BRect r;

			// Adding the menus
				// Adding the menu bar
		r = Bounds();
		r.bottom = 20;
		menubar = new BMenuBar (
			r,
			"menubar");
				
//		menubar->AttachedToWindow();
		BWindow::AddChild(menubar);	
		
			// Nullify number of used layers
		overall_number = 0;
		
				// Adding menus
					// Menu "File"
		M_file = new BMenu("File");
						// Adding "Clear" option, which does the same as button "Clear"
		M_file->AddItem(new BMenuItem("Clear", new BMessage(MENU_FILE_CLEAR), 'N'));
		M_file->AddSeparatorItem();
						// Adding options "Open" (which loads the bachground) and "Save as"
		M_file->AddItem(new BMenuItem("Open as background...", new BMessage(MENU_FILE_OPEN), 'O'));
		M_file->AddItem(new BMenuItem("Save As...", new BMessage(MENU_FILE_SAVEAS), 'S'));
		M_file->AddSeparatorItem();
						// Adding options that will be used in future for printing
		BMenuItem* PageSetup = new BMenuItem("Page Setup", new BMessage(MENU_FILE_PAGESETUP), 'G');
		PageSetup->SetEnabled(false);
		M_file->AddItem(PageSetup);
		BMenuItem* Print = new BMenuItem("Print...", new BMessage(MENU_FILE_PRINT), 'P');
		Print->SetEnabled(false);
		M_file->AddItem(Print);
		M_file->AddSeparatorItem();
						// Adding "Quit" option.
		M_file->AddItem(new BMenuItem("Quit", new BMessage(MENU_FILE_QUIT), 'Q'));
						// This line actually adds the menu
		menubar->AddItem(M_file);		
		
					// Menu "Layers"
		M_layers = new BMenu ("Layers");
						// Adding "Background" submenu
						// This submenu differs from other submenus by the fact that
						// it has the "Load..." option. "Delete" is disabled by default.
						// It will become enabled as soon as we'll have something to delete.
		layers[0] = new LayerItem ("Background", 0);
		layers[0]->AddItem(new BMenuItem("Load image...", new BMessage (MENU_FILE_OPEN), 'O') );
		layers[0]->del->SetEnabled(false);
		M_layers->AddItem( layers[0] );
		M_layers->AddSeparatorItem();
						// Nullifying pointers to other layer items in the menu "Layers"
		for (i=1; i<=MAX_LAYERS; i++) {
			layers[i] = NULL;
		}
		
		menubar->AddItem (M_layers);
		
					// Menu "Background"
		M_backgr = new BMenu ("Background");
						// Adding items that will load and delete the image
		M_backgr->AddItem(new BMenuItem ("Load image...", new BMessage (MENU_FILE_OPEN), 'O') );
		BMessage* MI_remove = new BMessage (DELETE_LAYER);
		MI_remove->AddPointer("sender", layers[0]);
		MI_del = new BMenuItem ("Remove", MI_remove, 0);
		MI_del->SetEnabled(false);
		M_backgr->AddItem(MI_del);
		M_backgr->AddSeparatorItem();
						// Adding itmes that will change background color
		M_backgr->AddItem(new BMenuItem ("Red", new BMessage (BG_COLOR_RED), 0) );
		M_backgr->AddItem(new BMenuItem ("Green", new BMessage (BG_COLOR_GREEN), 0) );
		M_backgr->AddItem(new BMenuItem ("Blue", new BMessage (BG_COLOR_BLUE), 0) );
		M_backgr->AddItem(new BMenuItem ("Cyan", new BMessage (BG_COLOR_CYAN), 0) );
		M_backgr->AddItem(new BMenuItem ("Yellow", new BMessage (BG_COLOR_YELLOW), 0) );		
		M_backgr->AddItem(new BMenuItem ("Pink", new BMessage (BG_COLOR_PINK), 0) );
		BMenuItem* MI_grey = new BMenuItem ("Grey", new BMessage (BG_COLOR_GREY), 0);
		M_backgr->AddItem(MI_grey);
		menubar->AddItem(M_backgr);
		
			// Adding "Color" BBox
		BBox* color = new BBox (BRect(3, 30, 100, 235), "color");
		color->SetLabel("Color");
		color->AttachedToWindow();
		MainView->AddChild(color);
		
			// Filling it with radiobuttons
				// Red
		BMessage* red = new BMessage('red');
		BRadioButton* redB = new BRadioButton ( 
			BRect(3,10,95,22), 
			"red", 
			"Red", 
			red);
		redB->AttachedToWindow();
		redB->SetTarget(this);
		redB->ResizeToPreferred();
		color->AddChild(redB);
		
				// Green
		BMessage* green = new BMessage('gre');
		BRadioButton* greenB = new BRadioButton ( 
			BRect(3,27,95,35), 
			"green", 
			"Green", 
			green);
		greenB->AttachedToWindow();
		greenB->SetTarget(this);
		greenB->ResizeToPreferred();
		color->AddChild(greenB);
		
				// Blue	
		BMessage* blue = new BMessage('blu');
		BRadioButton* blueB = new BRadioButton ( 
			BRect(3,44,95,54), 
			"blue", 
			"Blue", 
			blue);
		blueB->AttachedToWindow();
		blueB->SetTarget(this);
		blueB->ResizeToPreferred();
		color->AddChild(blueB);
		
				// Yellow
		BMessage* yellow = new BMessage('yel');
		BRadioButton* yellowB = new BRadioButton (
			BRect(3, 61,95,71),
			"yellow",
			"Yellow",
			yellow);
		yellowB->AttachedToWindow();
		yellowB->SetTarget(this);
		yellowB->ResizeToPreferred();
		color->AddChild(yellowB);
		
				// Violet
		BMessage* violet = new BMessage ('vio');
		BRadioButton* violetB = new BRadioButton (
			BRect(3,78,95,88),
			"violet",
			"Violet",
			violet);
		violetB->AttachedToWindow();
		violetB->SetTarget(this);
		violetB->ResizeToPreferred();
		color->AddChild(violetB);

				// Orange
		BMessage* orange = new BMessage ('ora');
		BRadioButton* orangeB = new BRadioButton (
			BRect(3,95,95,105),
			"orange",
			"Orange",
			orange);
		orangeB->AttachedToWindow();
		orangeB->SetTarget(this);
		orangeB->ResizeToPreferred();
		color->AddChild(orangeB);
		
				// Rainbow
		BMessage* rainbow = new BMessage ('rai');
		BRadioButton* rainbowB = new BRadioButton (
			BRect(3, 112,95,122),
			"rainbow",
			"Rainbow",
			rainbow);
		rainbowB->AttachedToWindow();
		rainbowB->SetTarget(this);
		rainbowB->ResizeToPreferred();
			// Disabling it for now
		rainbowB->SetEnabled(false);
		color->AddChild(rainbowB);				
		
				// Random
		BMessage* random = new BMessage ('ran');
		BRadioButton* randomB = new BRadioButton (
			BRect(3, 129,95,137),
			"random",
			"Random",
			random);
		randomB->SetValue(0);
		randomB->AttachedToWindow();
		randomB->SetTarget(this);
		randomB->ResizeToPreferred();
			// Disabling it now...
//		randomB->SetEnabled(false);
		color->AddChild(randomB);	
		
				// Pen width slider
		pen_width = new BSlider (
			BRect(3, 148, 95, 20),
			"pen_width",
			"Pen Width",
			NULL,
			1,
			10,
			B_TRIANGLE_THUMB);
//		pen_width->AttachedToWindow();
		pen_width->SetPosition(0);
		pen_width->SetLimitLabels("1", "10");
		pen_width->SetKeyIncrementValue(1);
				// Adding the color to the slider bar
					// Right side color
		temp_color.red = 0;
		temp_color.green = 215;
		temp_color.blue = 185;
		pen_width->SetBarColor(temp_color);
					// Left side color
		temp_color.red = 255;
		temp_color.green = 165;
		temp_color.blue = 185;
		pen_width->UseFillColor(true, &temp_color);
		pen_width->SetHashMarkCount(10);
		pen_width->SetHashMarks(B_HASH_MARKS_BOTH);
//		pen_width->ResizeToPreferred();
		pen_width->SetLowColor(219,219,219);
		pen_width->SetViewColor(219,219,219);
		color->AddChild(pen_width);		
		
		
		
		
		
			// Adding 3 text boxes for entering the radii
				// Main Radius
		text_to_add = new char[4];
		sprintf(text_to_add, "%d", main_R);
		main_rad = new BTextControl (
			BRect (3, 235, 98, 20),
			"main_rad",
			"Main Radius",
			text_to_add,
			0);
		main_rad->AttachedToWindow();		
		main_rad->SetDivider(60);
		BWindow::AddChild(main_rad);
		
				// Sub-radius (radius of the 3rd gear)
		sprintf(text_to_add, "%d", sub_R);
		sub_rad = new BTextControl (
			BRect (3, 255, 98, 20),
			"sub_rad",
			"3rd Gear",
			text_to_add,
			0);
		sub_rad->AttachedToWindow();		
		sub_rad->SetDivider(60);
		BWindow::AddChild(sub_rad);
		sub_rad->SetEnabled(false);		
		
				// Internal Radius
		sprintf(text_to_add, "%d", int_R);
		int_rad = new BTextControl (
			BRect (3, 275, 98, 20),
			"int_rad",
			"Int. Radius",
			text_to_add,
			0);
		int_rad->AttachedToWindow();
		int_rad->SetDivider(60);
		BWindow::AddChild(int_rad);
		
				// Hole position
		sprintf(text_to_add, "%d", hole_P);
		hole_pos = new BTextControl (
			BRect (3, 295, 98, 20),
			"hole_pos",
			"Hole Position",
			text_to_add,
			0);
		hole_pos->AttachedToWindow();
		hole_pos->SetDivider(60);
		BWindow::AddChild(hole_pos);
		
		
		
		
		
		
			// Adding buttons to draw and to clear the bitmap
				// Draw button
		BMessage* draw_butt = new BMessage (DRAW_BUTTON);
		drawB = new BButton(
			BRect (20, 330, 80, 30),
			"draw",
			"Draw!",
			draw_butt);
		drawB->AttachedToWindow();
		drawB->SetTarget(this);
		BWindow::AddChild(drawB);
		drawB->MakeDefault(true);
		
				// Clear button
		BMessage* clear_butt = new BMessage (MENU_FILE_CLEAR);
		clearB = new BButton(
			BRect (20, 370, 80, 30),
			"clear",
			"Clear",
			clear_butt);
		clearB->AttachedToWindow();
		clearB->SetTarget(this);
		BWindow::AddChild(clearB);
		
			// Fixed angle checkbox
		BMessage* ang = new BMessage ('ang');
		angle = new BCheckBox (
			BRect (3, 405, 97, 20),
			"angle",
			"Fixed angle",
			ang);
		angle->AttachedToWindow();
		angle->ResizeToPreferred();
		angle->SetTarget(this);
		angle->SetValue(1);
		BWindow::AddChild(angle);
		
			// 3rd gear checkbox
		BMessage* trd = new BMessage ('trd');
		trdg = new BCheckBox (
			BRect (3, 425, 97, 20),
			"trdg",
			"Third gear",
			trd);
		trdg->AttachedToWindow();
		trdg->ResizeToPreferred();
		trdg->SetTarget(this);
		trdg->SetValue(0);
		BWindow::AddChild(trdg);
		

		
		
		

			// Initialize the bitmap

				// Creating the view that will show info on screen	
		Drawing = new DrawView (
			BRect(101,25,600,526),
			"Drawing",
			B_FOLLOW_LEFT | B_FOLLOW_TOP,
			B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_SUBPIXEL_PRECISE);
				// Initializing the default background color as grey
		MI_grey->SetMarked(true);
		Drawing->bgcolor.red = 216;
		Drawing->bgcolor.green = 216;
		Drawing->bgcolor.blue = 216;
//		this->image = Drawing->image;
				// Nullifying the layers and the background
		for (i=0; i<MAX_LAYERS; i++) {
			Drawing->layers[i] = NULL;
		}
		
				// Create the view to draw into the bitmap
		im_bit = new BView (
			BRect(0, 0, 500, 503),
			"im_bit",
			B_FOLLOW_NONE,
			B_PULSE_NEEDED | B_FULL_UPDATE_ON_RESIZE | B_SUBPIXEL_PRECISE);
//		Drawing->image->AddChild(im_bit);		
				// Filling the bitmap with the default color
		Colour.red = 216;
		Colour.green = 216;
		Colour.blue = 216;
		for (i=0; i<(500*503*4); i+=4) {
			Drawing->image->SetBits(&Colour, 1, i, B_RGB32);
		}

		this->AddChild(Drawing);		
		Drawing->DrawBitmap(Drawing->image);
		
			// Initializing the starting color as red	
		BWindow::AddChild(MainView);
		redB->KeyDown(" ", 1);
		
			// Initializing the Layers menu
		Rebuild_Layers();
		
			// Initializing file save panel
		savepanel = new TranslatorSavePanel("TranslatorSavePanel", new BMessenger(this), NULL, 0, false,
			new BMessage(SAVE_FILE_PANEL));
			// Initializing file open panel
		openpanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL, 0, false,
			new BMessage(OPEN_FILE_PANEL), NULL, false, true);

	}
	else
	{
		AppReturnValue = B_NO_MEMORY;
		be_app->PostMessage(B_QUIT_REQUESTED);
	}
}

clsMainWindow::~clsMainWindow()
{
}

void clsMainWindow::MessageReceived(BMessage * Message)
{
	BMessage* newM;	
	LayerItem* layer_to_be_deleted;
	BRadioButton* random;
	int i;

	compact_layers();
	Message->PrintToStream();
	entry_ref ref;
	switch(Message->what)
	{
		case 'red':
			Colour.red = 255;
			Colour.green = 0;
			Colour.blue = 0;

			break;
		case 'gre':
			Colour.red = 0;
			Colour.green = 255;
			Colour.blue = 0;
			break;
		case 'blu':
			Colour.red = 0;
			Colour.green = 0;
			Colour.blue = 255;
			break;
		case 'yel':
			Colour.red = 255;
			Colour.green = 255;
			Colour.blue = 0;
			break;		
		case 'ora':
			Colour.red = 255;
			Colour.green = 140;
			Colour.blue = 0;
			break;		
		case 'vio':
			Colour.red = 190;
			Colour.green = 0;
			Colour.blue = 140;
			break;		
		
		case 'rai':     // There's no support for "Rainbow" now
			break;
		
		case 'ran':		// Random color is generated in other place (see case DRAW_BUTTON).
			random = (BRadioButton*)this->FindView("random");
			random->SetValue(1);
			break;
			
		case 'trd':
				// If user clicks the "3rd Gear" checkbox, I change the status
				// of the control to opposite of what it is now.
			if (trdg->Value() == 0) {
				sub_rad->SetEnabled(false);
			}
			else {
				sub_rad->SetEnabled(true);
			}
			break;
		case BG_COLOR_RED:
			if ( M_backgr->FindMarked() != NULL ) {
				(M_backgr->FindMarked())->SetMarked(false);
			}
			if ( M_backgr->FindItem ("Red") != NULL ) {
				(M_backgr->FindItem ("Red"))->SetMarked(true);
			}
			Drawing->bgcolor.red = 250;
			Drawing->bgcolor.green = 0;
			Drawing->bgcolor.blue = 0;
			Drawing->Draw();
			break;
		
		case BG_COLOR_GREEN:
			if ( M_backgr->FindMarked() != NULL ) {
				(M_backgr->FindMarked())->SetMarked(false);
			}
			if ( M_backgr->FindItem ("Green") != NULL ) {
				(M_backgr->FindItem ("Green"))->SetMarked(true);
			}
			Drawing->bgcolor.red = 0;
			Drawing->bgcolor.green = 250;
			Drawing->bgcolor.blue = 0;
			Drawing->Draw();
			break;
			
		case BG_COLOR_BLUE:
			if ( M_backgr->FindMarked() != NULL ) {
				(M_backgr->FindMarked())->SetMarked(false);
			}
			if ( M_backgr->FindItem ("Blue") != NULL ) {
				(M_backgr->FindItem ("Blue"))->SetMarked(true);
			}
			Drawing->bgcolor.red = 0;
			Drawing->bgcolor.green = 0;
			Drawing->bgcolor.blue = 250;
			Drawing->Draw();
			break;
		
		case BG_COLOR_GREY:
			if ( M_backgr->FindMarked() != NULL ) {
				(M_backgr->FindMarked())->SetMarked(false);
			}
			if ( M_backgr->FindItem ("Grey") != NULL ) {
				(M_backgr->FindItem ("Grey"))->SetMarked(true);
			}
			Drawing->bgcolor.red = 216;
			Drawing->bgcolor.green = 216;
			Drawing->bgcolor.blue = 216;
			Drawing->Draw();
			break;
			
		case BG_COLOR_CYAN:
			if ( M_backgr->FindMarked() != NULL ) {
				(M_backgr->FindMarked())->SetMarked(false);
			}
			if ( M_backgr->FindItem ("Cyan") != NULL ) {
				(M_backgr->FindItem ("Cyan"))->SetMarked(true);
			}
			Drawing->bgcolor.red = 250;
			Drawing->bgcolor.green = 250;
			Drawing->bgcolor.blue = 180;
			Drawing->Draw();
			break;
			
		case BG_COLOR_YELLOW:
			if ( M_backgr->FindMarked() != NULL ) {
				(M_backgr->FindMarked())->SetMarked(false);
			}
			if ( M_backgr->FindItem ("Yellow") != NULL ) {
				(M_backgr->FindItem ("Yellow"))->SetMarked(true);
			}
			Drawing->bgcolor.red = 250;
			Drawing->bgcolor.green = 250;
			Drawing->bgcolor.blue = 0;
			Drawing->Draw();
			break;
			
		case BG_COLOR_PINK:
			if ( M_backgr->FindMarked() != NULL ) {
				(M_backgr->FindMarked())->SetMarked(false);
			}
			if ( M_backgr->FindItem ("Pink") != NULL ) {
				(M_backgr->FindItem ("Pink"))->SetMarked(true);
			}
			Drawing->bgcolor.red = 250;
			Drawing->bgcolor.green = 170;
			Drawing->bgcolor.blue = 250;
			Drawing->Draw();
			break;
			
		case DRAW_BUTTON:
			newM = new BMessage(STOP_DRAWING);
			this->drawB->SetLabel("Pause");
			this->drawB->SetMessage(newM);
			this->Sync();
			random = (BRadioButton*)this->FindView("random");
			if ( (random != NULL) && (random->Value() != 0) ) {
				srand(1);
				Colour.red = rand() % 255;
				Colour.green = rand() % 255;
				Colour.blue = rand() % 255;
			}
			threadID=spawn_thread(DrawImage, "Drawer", 100, this);
			if (threadID > 0) {
				resume_thread(threadID);				
			}
			else {
				BAlert* cantDraw = new BAlert ("cantDraw","Can't draw the curve!","Ok");
				cantDraw->Go();
			}
			break;
			
		case DRAWING_COMPLETE:
			threadID = 0;			
			newM = new BMessage(DRAW_BUTTON);
			this->drawB->SetLabel("Draw!");
			this->drawB->SetMessage(newM);
			Rebuild_Layers();
//			Sync();
			break;
		case STOP_DRAWING:
			if (threadID > 0) {
				kill_thread(threadID);
			}
			else {
				threadID = 0;
				BAlert* cantQuit = new BAlert ("cantStop", "Can't stop drawing the curve, since nothing draws it!","Ok");
				cantQuit->Go();
			}
			newM = new BMessage(DRAW_BUTTON);
			this->drawB->SetLabel("Draw!");
			this->drawB->SetMessage(newM);
			break;
				
		case MENU_FILE_CLEAR:
			
			for (i=0; i < MAX_LAYERS; i++) {
				if (layers[i+1] != NULL) {
					M_layers->RemoveItem(layers[i+1]);					
					delete layers[i+1];
					layers[i+1] = NULL;
				}								
				if (Drawing->layers[i] != NULL) {
					delete Drawing->layers[i];
					Drawing->layers[i] = NULL;
				}
			}			
			Drawing->Draw();
			break;
		
		case MENU_FILE_QUIT:
			this->QuitRequested();
			break;
		
		case MENU_FILE_SAVEAS:
			savepanel->Window()->SetWorkspaces(B_CURRENT_WORKSPACE);
			savepanel->Show();

			break;
			
		case SAVE_FILE_PANEL:
			this->SaveImage(Message);
		
			break;
			
		case MENU_FILE_OPEN:
			openpanel->Window()->SetWorkspaces(B_CURRENT_WORKSPACE);
			openpanel->Show();
			break;
			
		case OPEN_FILE_PANEL:		// Fallthrough
		case B_SIMPLE_DATA:
			this->SetImage(Message);
			this->layers[0]->del->SetEnabled(true);
			MI_del->SetEnabled(true);
			break;
			
		case BRING_TO_FRONT:
			if ((this->Lock()) && (Drawing->LockLooper())) {
					// Instead of "layer_to_be_deleted" read "layer_to_be_moved" :-)
				Message->FindPointer("sender", (void **) &layer_to_be_deleted);
				Bring_to_front(layer_to_be_deleted);
				this->Unlock();
				Drawing->UnlockLooper();
				Drawing->Draw();
			}
			else {
				BAlert* cantLockAnything = new BAlert (
				NULL, 
				"Couldn't lock the window or the image!",
				"Ok");
				cantLockAnything->Go();
			}
			break;
			
		case SEND_TO_BACK:
			if ((this->Lock()) && (Drawing->LockLooper())) {
					// Instead of "layer_to_be_deleted" read "layer_to_be_moved" :-)
				Message->FindPointer("sender", (void **) &layer_to_be_deleted);
				Send_to_back(layer_to_be_deleted);
				this->Unlock();
				Drawing->UnlockLooper();
				Drawing->Draw();
			}
			else {
				BAlert* cantLockAnything = new BAlert (
				NULL, 
				"Couldn't lock the window or the image!",
				"Ok");
				cantLockAnything->Go();
			}
			break;	
		
		case BRING_FORWARD:
			if ((this->Lock()) && (Drawing->LockLooper())) {
					// Instead of "layer_to_be_deleted" read "layer_to_be_moved" :-)
				Message->FindPointer("sender", (void **) &layer_to_be_deleted);
				Swap_layers(layer_to_be_deleted->num_layer, layer_to_be_deleted->num_layer+1);
				this->Unlock();
				Drawing->UnlockLooper();
				Drawing->Draw();
			}
			else {
				BAlert* cantLockAnything = new BAlert (
				NULL, 
				"Couldn't lock the window or the image!",
				"Ok");
				cantLockAnything->Go();
			}
			break;
			
		case SEND_BACKWARD:
			if ((this->Lock()) && (Drawing->LockLooper())) {
					// Instead of "layer_to_be_deleted" read "layer_to_be_moved" :-)
				Message->FindPointer("sender", (void **) &layer_to_be_deleted);
				Swap_layers(layer_to_be_deleted->num_layer, layer_to_be_deleted->num_layer-1);
				this->Unlock();
				Drawing->UnlockLooper();
				Drawing->Draw();
			}
			else {
				BAlert* cantLockAnything = new BAlert (
				NULL, 
				"Couldn't lock the window or the image!",
				"Ok");
				cantLockAnything->Go();
			}
			break;
			
		case DELETE_LAYER:
			if (this->LockLooper() ) {
					// "layer_to_be_deleted" is a pointer to LayerItem we're deleting.
				Message->FindPointer("sender", (void **) &layer_to_be_deleted);
					// Checking all LayerItems in search for the one to delete
				for (i=0; i<MAX_LAYERS+1; i++) {
						// Found it!
					if (layers[i] == layer_to_be_deleted) {
					
					
							// If we're about to delete the background
						if (i==0) {	
							if (Drawing->LockLooper() ) 
							{
								delete (Drawing->bgimage);
								Drawing->bgimage = NULL;
								Drawing->UnlockLooper();
								Drawing->Draw();
								layers[i]->del->SetEnabled(false);
								MI_del->SetEnabled(false);
								Drawing->UnlockLooper();
							}
							else {
								BAlert* cantLock = new BAlert (NULL, "Can't lock the drawing!", "Ok");
								cantLock->Go();
							}
						}
						
						
						
							// Else we shall delete a "real" layer
						else {
												
								// Locking the drawing element
							if ( ( Drawing->LockLooper() ) ) {
									// Taking care of the bitmap
								if (Drawing->layers[i-1] != NULL) {
									delete Drawing->layers[i-1];
									Drawing->layers[i-1] = NULL;
									for (int k=i; k<MAX_LAYERS; k++) {
										Drawing->layers[k-1]=Drawing->layers[k];
									}
									Drawing->layers[MAX_LAYERS-1] = NULL;
									Drawing->Draw();
								}
									// Taking care of the menus
								if (layers[i] != NULL) {
										// Removing the LayerItem menu
									M_layers->RemoveItem(layers[i]);								
									delete (layers[i]);
									layers[i] = NULL;
										// Shifting other LayerItems
									for (int k=i; k<MAX_LAYERS-1; k++) {
										layers[k] = layers[k+1];
									}
										// Nullifying the last LayerItem
									layers[MAX_LAYERS] = NULL;
								}
								
								this->Unlock();
								Drawing->UnlockLooper();
							}
							else {
								BAlert* cantLoc = new BAlert (
									NULL, 
									"Can't lock 'Drawing' window element or the window itself!", 
									"Ok");
								cantLoc->Go();
							}
						}
					Rebuild_Layers();
					Drawing->Draw();
					break;
					}
				}
			}
			else {
				BAlert* cantLock = new BAlert (NULL, "Can't lock main window!", "Ok");
				cantLock->Go();
			}
			break;

		default:
		  	BWindow::MessageReceived(Message);
		  	break;
	}
}

bool clsMainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true; // BWindow::QuitRequested();
}

void clsMainWindow::SetImage (BMessage *message) {
	drag_from_tracker = false;
	entry_ref ref;
	if (message->FindRef("refs", &ref) != B_OK) return;
	BEntry entry(&ref);
	BPath path(&entry);
		
	BBitmap* new_bitmap = BTranslationUtils::GetBitmapFile(path.Path());
	
	// Only replace the old image and resize the window if the new image
	// was loaded sucessfully
	if (new_bitmap != NULL) {
		if (Drawing->bgimage != NULL) delete Drawing->bgimage;
		Drawing->bgimage = new_bitmap;
		
			// Draw the new image
		this->Drawing->Draw();
	} else {
		BAlert *alert = new BAlert(NULL, "Could not load the image.", "OK");
		alert->Go();
	}
}

void clsMainWindow::SaveImage(BMessage *message) {
	// Recover the necessary data from the message
	translator_id *id;
	uint32 format;
	ssize_t length = sizeof(translator_id);
	if (message->FindData("translator_id", B_RAW_TYPE, (const void **)&id, &length) != B_OK) return;
	if (message->FindInt32("translator_format", (int32 *)&format) != B_OK) return;
	entry_ref dir;
	if (message->FindRef("directory", &dir) != B_OK) return;
	BDirectory bdir(&dir);
	const char *name;
	if (message->FindString("name", &name) != B_OK) return;
	if (name == NULL) return;
	
	// Clobber any existing file or create a new one if it didn't exist
	BFile file(&bdir, name, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK) {
		BAlert *alert = new BAlert(NULL, "Could not create file.", "OK");
		alert->Go();
		return;
	}
	
	BTranslatorRoster *roster = BTranslatorRoster::Default();
	BBitmapStream stream(Drawing->image);
	
	// If the id is no longer valid or the translator fails for any other
	// reason, catch it here
	if (roster->Translate(*id, &stream, NULL, &file, format) != B_OK) {
		BAlert *alert = new BAlert(NULL, "Could not save the image.", "OK");
		alert->Go();
	}
	
	// Reclaim the ownership of the bitmap, otherwise it would be deleted
	// when stream passes out of scope
	stream.DetachBitmap(&(Drawing->image));
}

void clsMainWindow::function (double* x, 
							  double* y, 
							  int main_R, 
							  int int_R, 
							  int hole_P, 
							  double deg) 
{
	*x = ( (main_R - int_R)*cos(deg) + hole_P*cos ((main_R*deg)/int_R) );
	*y = ( (main_R - int_R)*sin(deg) - hole_P*sin ((main_R*deg)/int_R) );
	return;
}

int clsMainWindow::GCD (int a, int b) {
	int m, n;
		// Initialization
	if (a >= b) { m = a;	n = b; }
	else        { m = b;	n = a; }	
		// Recursion stop
	if (n == 0) { return m; }	
		// Recursion cont
	else { 
		return ( GCD (n, (m%n) ) );
	}
}


	// This relatively heavy function compacts the layers arrays by
	// moving unnecessary zeros (NULLs) to the end of the array.
void clsMainWindow::compact_layers (void) {
	int i=1, j=1;
	bool finished = true;
		// Skipping layers until the first unused (NULL)
	while ( (i <= MAX_LAYERS) ) {
		if ( layers[i] != NULL) {
			if (Drawing->layers[i-1] == NULL) {
				BAlert* incons = new BAlert (NULL, "Inconsistency between layer menu and bitmap!", "Ok");
				incons->Go();
				return;
			}
			layers[i]->num_layer = i;
			i++;			
		} else {
			if (Drawing->layers[i-1] != NULL) {
				BAlert* incons = new BAlert (NULL, "Inconsistency between bitmap and layer menu!", "Ok");
				incons->Go();
				return;
			}
			for (j=i, finished = true; j<MAX_LAYERS; j++) {
					// In order to exit from what may be infinite loop, I use variable
					// "finished". If it remains "true" after the following loop - it means
					// all the rest of menus dont' exist, and the arrays are compacted.
					// POSSIBLE BUG: I don't check here that bitmap layers are compacted too!
				if (layers[j] != NULL) { finished = false; }
				layers[j] = layers[j+1];
				Drawing->layers[j-1] = Drawing->layers[j];
				layers[MAX_LAYERS] = NULL;
				Drawing->layers[MAX_LAYERS-1] = NULL;
			}
			if (finished) { return; }
			
		}	
	}
	return;
}


	// This function moves a layer to the top of the layers
void clsMainWindow::Bring_to_front (LayerItem* to_be_first) {

		// At first, it searches for the layer to move.
	for (int i=1; i<MAX_LAYERS+1; i++) {
		if (to_be_first == layers[i] ) {
				// Now the layer is found. Its LayerItem menu is no. i,
				// and corresponding bitmap is Drawing->layers[i-1].
			layers[overall_number] = layers[i];
			Drawing->layers[overall_number - 1] = Drawing->layers[i-1];
				// Nullify the places from where we've moved the layers
			layers[i] = NULL;
			Drawing->layers[i-1] = NULL;
				// Remove unnecessary NULLs from the arrays
			compact_layers();
				// Redraw the image
			Drawing->Draw();
			break;          // Exitting the loop - we've did what we had to.
		}
	}
	Rebuild_Layers();	
}


	// This function moves a layer to the bottom of the layers
void clsMainWindow::Send_to_back (LayerItem* to_be_last) {
	int i = 1;
		// Searching for the layer
	for ( ; i<MAX_LAYERS+1; i++) {
		if (layers[i] == to_be_last) {
			while (i>1) {
				Swap_layers(i-1, i);
				i--;
			}
			break;
		}		
	}
	compact_layers();
	Rebuild_Layers();	
}


	// These functions swaps two layers.
void clsMainWindow::Swap_layers (LayerItem* a, LayerItem* b) {
		// Fool protection
	if ( (a == b) || (a == NULL) || (b == NULL) ) {
		return;
	}
	int no_a=0, no_b=0, i=1;
		// Searching for the layers to swap.
	for ( ; (i < MAX_LAYERS+1); i++) {
		if (layers[i] == a) { no_a = i; }
		if (layers[i] == b) { no_b = i; }
			// If both layers were found, no need to seek further.
		if ((no_a != 0) && (no_b != 0)) { break; } 
	}
		// Swapping itself
	LayerItem* temp = layers[no_a];
	layers[no_a] = layers [no_b];
	layers[no_b] = temp;
	BBitmap* temp2 = Drawing->layers[no_a - 1];
	Drawing->layers[no_a - 1] = Drawing->layers[no_b - 1];
	Drawing->layers[no_b - 1] = temp2;
	Rebuild_Layers();	
}
	
void clsMainWindow::Swap_layers (int no_a, int no_b) {
		// Fool protection
	if ( (no_a == no_b) || 
	     (no_a <= 0)    || 
		 (no_b <= 0)    || 
		 (no_a > MAX_LAYERS+1) || 
		 (no_b > MAX_LAYERS+1) 
	) {
		return;
	}
		// Swapping itself
	LayerItem* temp = layers[no_a];
	layers[no_a] = layers [no_b];
	layers[no_b] = temp;
	BBitmap* temp2 = Drawing->layers[no_a - 1];
	Drawing->layers[no_a - 1] = Drawing->layers[no_b - 1];
	Drawing->layers[no_b - 1] = temp2;
	Rebuild_Layers();
}	

	// This function rebuilds the "Layers" menu.
void clsMainWindow::Rebuild_Layers (void) {
	int i;
	int last_menu=0;
	
	if ( this->Lock() ) {
		for (i=1; i<=MAX_LAYERS; i++) {
			if (layers[i] != NULL) {
				M_layers->RemoveItem(2);
			}
		}
		for (i=1; i<=MAX_LAYERS; i++) {
			if (layers[i] != NULL) {
				M_layers->AddItem(layers[i]);
				layers[i]->bring_fw->SetEnabled(true);
				layers[i]->bring_front->SetEnabled(true);
				layers[i]->send_bw->SetEnabled(true);
				layers[i]->send_back->SetEnabled(true);
				last_menu = i;
			}
		}
		if (last_menu != 0) {
			layers[last_menu]->bring_fw->SetEnabled(false);
			layers[last_menu]->bring_front->SetEnabled(false);
		}
		if (layers[1] != NULL) {
			layers[1]->send_bw->SetEnabled(false);
			layers[1]->send_back->SetEnabled(false);
		}
		this->Unlock();
	}
	else {
		BAlert* cantLock = new BAlert (NULL, "Couldn't lock the window!", "So use a heavier hammer!");
		cantLock->Go();
	}
}

int clsMainWindow::DrawImage(void* _data) {
	clsMainWindow* tis = (clsMainWindow*)_data;
	int m_ang;
	char* text;
//	int i;
	double deg = 0, third_deg = 0;
	double x=0, y=0;
	double x_mid=350, y_mid=250, offset;
	double x_pr, y_pr;
				// Updating the curve parameters (with fool protection)
			tis->main_R = atoi(tis->main_rad->Text());
			if (tis->main_R > 250)    { tis->main_R = 250; }
			else if (tis->main_R < 2) { tis->main_R =   2; }
					// If 3rd gear is used, taking it into account
			if (tis->trdg->Value() != 0) {
				tis->sub_R = atoi(tis->sub_rad->Text());
				if (tis->sub_R >= tis->main_R)    { tis->sub_R = tis->main_R-1; }
				else if (tis->sub_R < 3)          { tis->sub_R =        3; }
				tis->int_R=atoi(tis->int_rad->Text());
				if (tis->int_R >= tis->sub_R)     {tis->int_R =  tis->sub_R-1; }
				else if (tis->int_R < 1)          {tis->int_R =        1; }
				tis->hole_P=atoi(tis->hole_pos->Text());
				if (tis->hole_P >= tis->int_R)    { tis->hole_P = tis->int_R-1; }
				else if (tis->hole_P < 1)         { tis->hole_P =       0; }
			}
					// Else the 3rd gear is not used
			else {
				tis->int_R=atoi(tis->int_rad->Text());
				if (tis->int_R >= tis->main_R) {tis->int_R = tis->main_R-1; }
				else if (tis->int_R < 1)       {tis->int_R =        1; }
				tis->hole_P=atoi(tis->hole_pos->Text());
				if (tis->hole_P >= tis->int_R)  { tis->hole_P = tis->int_R-1; }
				else if (tis->hole_P < 1)       { tis->hole_P =       0; }
			}
				// Recalculating current tis->GCD
			tis->cur_GCD = tis->GCD (tis->main_R, tis->int_R);

						
				// Redisplaying the paratemers (so the user will know what will be drown)
			text = new char[4];
			sprintf(text, "%d", tis->main_R);
			if (tis->main_rad->LockLooper() ) {
				tis->main_rad->SetText(text);
				tis->main_rad->UnlockLooper();
			}
					// Including the 3rd gear radius, if needed
			if (tis->trdg->Value() != 0) {
				sprintf(text, "%d", tis->sub_R);
				if (tis->sub_rad->LockLooper() ) {
					tis->sub_rad->SetText(text);
					tis->sub_rad->UnlockLooper();
				}
			}
			sprintf(text, "%d", tis->int_R);
			if (tis->int_rad->LockLooper() ) {
				tis->int_rad->SetText(text);
				tis->int_rad->UnlockLooper();
			}

			sprintf(text, "%d", tis->hole_P);
			if (tis->hole_pos->LockLooper() ) {
				tis->hole_pos->SetText(text);
				tis->hole_pos->UnlockLooper();
			}
			delete text;

			if (tis->Drawing->layers[overall_number-1] != NULL ) {
//				delete tis->Drawing->layers[overall_number];
//				tis->Drawing->layers[overall_number] = NULL;
				BAlert* alerting = new BAlert (NULL, "The bitmap layer is already taken!", "Ok");
				alerting->Go();
			}
			tis->Drawing->layers[overall_number-1] = new BBitmap (
				BRect (0, 0, 500, 501), 
				B_RGB32, 
				true, 
				true);
			if (tis->Drawing->layers[overall_number-1] != NULL) {
				tis->Drawing->layers[overall_number-1]->AddChild(tis->im_bit);			
			} else {
				BAlert* cantBitmap = new BAlert (NULL, "Can't allocate new layer bitmap!", "Ok");
			}
			

			
//			tis->im_bit->UnlockLooper();


				// Drawing the curve!
			if ( tis->im_bit->LockLooper() ) {
				
				int32 Transparent_Pixel = B_TRANSPARENT_MAGIC_RGBA32;
	
					// Filling the bitmap with transparent color
				for (int i=0; i<tis->Drawing->layers[overall_number-1]->BitsLength(); i+=4) {
					tis->Drawing->layers[overall_number-1]->SetBits(
						&Transparent_Pixel,
						4,
						i,
						B_RGB32);
				}
				
				
						// IMPORTANT: return value check needed!
				tis->im_bit->SetViewColor(0,0,0);
				tis->im_bit->SetLowColor(0,0,0);
				
							// Taking appropriate actions for "Random" color
				long now;
				srand (time (&now) % 37);
				BRadioButton* random = (BRadioButton*)tis->FindView("random");
				if ( (random != NULL) && (random->Value() != 0) ) {
					tis->Colour.red = rand() % 256;
					tis->Colour.blue = rand() % 256;
					tis->Colour.green = rand() % 256;
				} 
				tis->im_bit->SetHighColor(tis->Colour);
				tis->im_bit->SetPenSize(tis->pen_width->Value());
					// If the user wants to perform fixed num of strokes,
					// setting m_ang to 500, else letting it go loose.
				if (tis->angle->Value() != 0) {
					m_ang = 500;
				}
				else {
					m_ang = 360*tis->int_R/tis->cur_GCD;
				}
			
				if (tis->trdg->Value() != 0) {
					offset = tis->main_R - tis->sub_R;
					y_mid = y_mid + offset;
				}
				else {
					offset = 0;
				} 
			
				for (deg=0 ; deg <= m_ang; deg += 0.01) {
					
					if (offset != 0) {
						tis->function(&x, &y, tis->sub_R, tis->int_R, tis->hole_P, deg);
						x_mid = 350 + cos(third_deg)*offset;
						y_mid = 250 + sin(third_deg)*offset;
						third_deg -= 0.1;
					}
					else {
						tis->function(&x, &y, tis->main_R, tis->int_R, tis->hole_P, deg);
					}
				
					x = x_mid + x;
					y = y_mid + y;
					if (deg == 0) { 
						x_pr = x; 
						y_pr = y; 					 
					}
					srand (time (&now) % 37);
					if ( (random != NULL) && (random->Value() != 0) ) {
						tis->Colour.red = abs(tis->Colour.red + (rand()%30 - 15)) % 256; 
						tis->Colour.blue = abs(tis->Colour.blue + (rand()%30 - 15)) % 256;
						tis->Colour.green = abs(tis->Colour.green + (rand()%30 - 15)) % 256;
						tis->im_bit->SetHighColor(tis->Colour);						
					}
					tis->im_bit->StrokeLine(BPoint(x_pr-100, y_pr), BPoint(x-100,y));
									
					x_pr = x;
					y_pr = y;

				}
				tis->im_bit->Sync();
				tis->im_bit->UnlockLooper();
				tis->Drawing->layers[overall_number-1]->RemoveChild(tis->im_bit);
				
					// Drawing complete. Now adding the LayerItem to "Layers" menu:
				text = new char[9];
				if (((BRadioButton*)tis->FindView("red"))->Value() != 0) {
					sprintf (text, "Red");
				}
				else if (((BRadioButton*)tis->FindView("green"))->Value() != 0) {
					sprintf (text, "Green");
				}
				else if (((BRadioButton*)tis->FindView("blue"))->Value() != 0) {
					sprintf (text, "Blue");
				}
				else if (((BRadioButton*)tis->FindView("yellow"))->Value() != 0) {
					sprintf (text, "Yellow");
				}
				else if (((BRadioButton*)tis->FindView("orange"))->Value() != 0) {
					sprintf (text, "Orange");
				}
				else if (((BRadioButton*)tis->FindView("random"))->Value() != 0) {
					sprintf (text, "Random");
				}
				else if (((BRadioButton*)tis->FindView("violet"))->Value() != 0) {
					sprintf (text, "Violet");
				}
				else {
					sprintf (text, "Unknown");
				}
				int position = overall_number;
				tis->layers[position] = new LayerItem (text, position);
				if (tis->layers[position-1] != NULL) {
					tis->M_layers->AddItem(tis->layers[position-1]);
				}
				delete text;
				
			}
			else {
				BAlert* im_bit_alert = new BAlert (NULL, "++ Can't lock the drawing element!", "Ok");
				im_bit_alert->Go();
			}
			if (tis->Drawing->LockLooper()) {
				tis->Drawing->Draw();
				tis->Drawing->UnlockLooper();
			}
			
			BMessage mess(DRAWING_COMPLETE);
			tis->PostMessage(&mess);
			exit_thread(0);
			return 0;
}

//===========================================================================
//		Implementation of the DrawView class
//===========================================================================

DrawView::DrawView (BRect rect, const char* name, uint32 res, uint32 flags) :
	BView (rect, name, res, flags)
{
	image = new BBitmap (
		BRect (0, 0, 500, 501),
		B_RGB32,
		true,
		true);
	if (image == NULL) {
		BAlert* cantMakeView = new BAlert (
			NULL, 
			"Couldn't create the image to be drawn!",
			"Ok");
		cantMakeView->Go();
	}
	bgimage = NULL;
}

void DrawView::Draw (BRect rect) {
	if (image) {
		SetDrawingMode(B_OP_OVER);
		this->DrawBitmap(image, rect, rect);
	}
}

	// This function creates the final image and shows it to the user.
	// This image will also be used in savings.
void DrawView::Draw(void) {
	long threadID;
	status_t Ret_Value;

		// If the bitmap we shall show does not exist, create it!
	if (this->image == NULL) 
	{
			// Allocating
		this->image = new BBitmap (
			BRect (0, 0, 500, 501),
			B_RGB32,
			true,
			true);
			// Checking that allocation was actually performed
		if (image == NULL) {
			BAlert* cantDraw = new BAlert (NULL, "Could not create the image!", "Ok");
			cantDraw->Go();
			return;
		}
	}
	
		// Allocating a new BView that will be used for drawing into the bitmap.
	BView* drawer = new BView (
		image->Bounds(), 
		"drawer", 
		B_FOLLOW_LEFT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_SUBPIXEL_PRECISE);
			// If allocated successfully, add it to the bitmap
	if (drawer != NULL) {
		image->AddChild(drawer);
	}
	else {	// or else, alerting the user:
		BAlert* cantDraw = new BAlert (NULL, "Could not create the image!", "Ok");
		cantDraw->Go();
		return;
	}
	
		// Now we have a new BView, and an empty bitmap.
		
		// First of all, let's fill in the background image!			
		// I fill the area behind the background image with background color,
		// in order to have some color behind transparent pixels in the back-
		// ground image (if it has any transparent pixels).
		//
		// I have to lock the looper of the background image.
	if ( drawer->LockLooper() ) {
		drawer->SetHighColor(bgcolor);
		drawer->FillRect (image->Bounds() );
		if ( bgimage != NULL ) {
			drawer->DrawBitmap (bgimage, image->Bounds() );
		}

		drawer->Sync(); 		// Only now the drawing was performed
		drawer->UnlockLooper();
	}
	else {			// Or else 
		BAlert* cantLockLooper = new BAlert (NULL, "Cannot ackuire the lock on the drawing element", "Ok");
		cantLockLooper->Go();
	}
		
	
		// We may safely destroy the helping "drawer" BView
	image->RemoveChild(drawer);
	delete drawer;
	drawer = NULL;
	
		// Invoking another thread to speed up the calculations
	threadID = spawn_thread (Rewrite_image, "rewriter", 100, this);
	if (threadID > 0) {
		resume_thread(threadID);
		wait_for_thread(threadID, &Ret_Value);
	}
	else {
		BAlert* cantMerge = new BAlert (NULL, "Can't invoke image merging thread!", "Too bad");
		cantMerge->Go();
	}		
	
		// Now the image bitmap is complete and is good to show
	this->DrawBitmap (image);
	return;
}
	

int DrawView::Rewrite_image (void* _data) {
	DrawView* data = (DrawView*)_data;
	rgb_color Transparent;
	memcpy (&Transparent, &B_TRANSPARENT_MAGIC_RGBA32, 4);

			// We'll add a drawing view to the bitmap
		int i;		// Index for the loop - we'll need it later
		BView* drawer = new BView (
			data->image->Bounds(), 
			"drawer", 
			B_FOLLOW_LEFT | B_FOLLOW_TOP,
			B_WILL_DRAW | B_SUBPIXEL_PRECISE);
				// If allocated successfully, add it to the bitmap
		if (drawer != NULL) {
			data->image->AddChild(drawer);
		}
		
				// Now, adding the image layers, one by one
				// The drawing mode must be set to B_OP_MODE, for the LowColor
				// of the drawing shall be treated as transparent.
				//
				// I must acquire the lock!
		if ( drawer->LockLooper() ) {
			drawer->SetLowColor(Transparent);
			drawer->SetViewColor(Transparent);
			drawer->SetDrawingMode(B_OP_OVER);
			for (i=0; i<MAX_LAYERS-1; i++) {
					// If current layer is not empty
				if ( (data->layers[i] != NULL) && ( (intptr_t )data->layers[i] != 0x19) ) {
						// Adding it to the bitmap, in B_OP_OVER mode
					drawer->DrawBitmap (data->layers[i]);
					drawer->Sync();
				}
			}
			drawer->UnlockLooper();
		}
		else {			// If I can't lock the BView, I alert the user.
			BAlert* cantLockLooper = new BAlert(NULL, "Cannot acquire the lock on the drawing element!", "Ok");
			cantLockLooper->Go();
		}
		
			// Now we may successfully delete the "drawer" BView
		data->image->RemoveChild (drawer);
		delete drawer;
		drawer = NULL;

			// Unlocking the lock
//		data->UnlockLooper();
//	}
//	else {		// We didn't succeed to take the lock,
//				// So we're alerting user about this
//		BAlert* cantLock = new BAlert (
//			NULL, 
//			"Could not lock the master drawing place, please try again!",
//			"Ok, if you say...");
//		cantLock->Go();
//	}
	
	exit_thread(0);
	return 0;
}

DrawView::~DrawView () {}
	
void DrawView::MessageReceived (BMessage* message) {
	switch (message->what) {
		case B_COPY_TARGET:
			GetTrackerResponse(message);
			break;
		case DRAG_TO_TRACKER: {
			BPoint point;
			if (message->FindPoint("point", &point) != B_OK) break;
			DragToTracker(point);
			break;
		}
		default:
			BView::MessageReceived(message);
			break;
	}
}

void DrawView::MouseDown(BPoint point) {
	drag_to_tracker = true;
	BMessage *message = new BMessage(DRAG_TO_TRACKER);
	message->AddPoint("point", point);
	messagerunner = new BMessageRunner(BMessenger(this), message, 250000, 1);
}

void DrawView::MouseUp(BPoint point) {
	drag_to_tracker = false;
}

void DrawView::DragToTracker(BPoint point) {
	delete messagerunner;
	messagerunner = NULL;
	if (!drag_to_tracker) return;
	drag_to_tracker = false;
	
	if (image == NULL) return;
	if (image->ColorSpace() != B_RGB32 && image->ColorSpace() != B_RGBA32) return;
	BBitmap *drag_bitmap = new BBitmap(image->Bounds(), B_RGBA32);
	if (drag_bitmap == NULL) return;
	uchar *bits = (uchar *)drag_bitmap->Bits();
	if (bits == NULL) {
		delete drag_bitmap;
		return;
	}
	
	int height = image->Bounds().IntegerHeight() + 1;
	int row_bytes = image->BytesPerRow();
	memcpy(bits, image->Bits(), row_bytes * height);
	int area = height * (image->Bounds().IntegerWidth() + 1);
	for (int x = 0; x < area; x++) {
		bits[3] = 0x7f;
		bits += 4;
	}
	
	BMessage message(B_SIMPLE_DATA);
	message.AddInt32("be:actions", B_COPY_TARGET);
	message.AddString("be:filetypes", "application/octet-stream");
	message.AddString("be:types", "application/octet-stream");
	message.AddString("be:clip_name", "Spirograph Bitmap");
	DragMessage(&message, drag_bitmap, B_OP_BLEND, point);
}


void DrawView::GetTrackerResponse(BMessage *message) {
	entry_ref dir_ref;
	if (message->FindRef("directory", &dir_ref) != B_OK) return;
	const char *filename;
	if (message->FindString("name", &filename) != B_OK) return;
	BDirectory dir(&dir_ref);
	BFile file(&dir, filename, B_WRITE_ONLY | B_CREATE_FILE);
	if (file.InitCheck() != B_OK) return;
	BBitmapStream stream(image);
	BTranslatorRoster *roster = BTranslatorRoster::Default();
	if (roster->Translate(&stream, NULL, NULL, &file, 'TGA ', 0) != B_OK) {
		BAlert *alert = new BAlert(NULL, "Could not drop image to Tracker.", "OK");
		alert->Go();
	}
	stream.DetachBitmap(&image);
}




//============================================================================
//    Implementation of the LayerItem class
//============================================================================

LayerItem::LayerItem (const char* name, int position) : BMenu (name)
{
		// MAX_LAYERS is actual number of layers with curves.
		// It does not include the background layer. But overall_number does!
		// Therefore we shall check current amount of layers against
		// MAX_LAYERS + 1.
	if (overall_number >= MAX_LAYERS + 1) {
		BAlert* cantCreate = new BAlert (
			NULL, 
			"Maximum number of layers exceeded, delete some first!",
			"Ok, I'll do it!");
		cantCreate->Go();
		return;
	}
	
		// Setting the layer number
	this->num_layer = position;
	overall_number++;
	
		// Preparing all the submenu items
	BMessage* M_bfr = new BMessage (BRING_TO_FRONT);
	M_bfr->AddPointer("sender", this);
	bring_front = new BMenuItem (
		"Bring to front",
		 M_bfr);
	BMessage* M_bfw = new BMessage (BRING_FORWARD);
	M_bfw->AddPointer("sender", this);
	bring_fw = new BMenuItem (
		"Bring forward",
		M_bfw );	
	BMessage* M_sbw = new BMessage (SEND_BACKWARD);
	M_sbw->AddPointer("sender", this);
	send_bw = new BMenuItem (
		"Send backward",
		M_sbw );
	BMessage* M_stb = new BMessage (SEND_TO_BACK);
	M_stb->AddPointer("sender", this);
	send_back = new BMenuItem (
		"Send to back",
		M_stb );
	BMessage* M_del = new BMessage (DELETE_LAYER);
	M_del->AddPointer("sender", this);
	del = new BMenuItem (
		"Delete layer",
		M_del );

		// Disabling all ordering options for the background
	if (position == 0) {
		bring_fw->SetEnabled(false);
		bring_front->SetEnabled(false);
		send_bw->SetEnabled(false);
		send_back->SetEnabled(false);
	}
		// Disabling "Bring forward" and "Bring to front" options in last layer
		// and managing other Enabling / Disabling options will be done by the window.
		
		// Adding the items
	this->AddItem (bring_front);
	this->AddItem (bring_fw);
	this->AddItem (send_bw);
	this->AddItem (send_back);
	this->AddSeparatorItem();
	this->AddItem (del);
	return;
}

LayerItem::~LayerItem (void) {
//	delete this->bring_front;
//	delete this->bring_fw;
//	delete this->send_bw;
//	delete this->send_back;
//	delete this->del;
//	bring_front = bring_fw = send_bw = send_back = del = 0;
	overall_number--;
} 
