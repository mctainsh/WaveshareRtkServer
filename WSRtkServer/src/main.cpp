#include <Arduino.h>
#include "Global.h"
#include "LV/LVCore.h"
#include "UI/screens/ui_MainScreen.h"
#include <LV/SwipePageGps.h>
#include <LV/SwipePageIO.h>
#include <SDFile.h>
#include <LV/SwipePageSettings.h>

SDFile _sdFile; // Create an instance of SDFile
LVCore _lvCore;				

// Pages
SwipePageGps _systemPageGps; 
SwipePageGps _swipePageGps; 
SwipePageIO _swipePageIO; 
SwipePageSettings _swipePageSettings; 



std::string _mdnsHostName; 

///////////////////////////////////////////////////////////////////////////////
// This function is called once at the beginning of the program
// It initializes the display, touch, and LVGL library
void setup()
{
	Serial.begin(115200); // Initialize serial communication for debugging
	while (!Serial); // Wait for serial port to connect (uncomment if needed)
	delay(100); // Wait for a short time to ensure the serial connection is established	
	Serial.setDebugOutput(true);
	Serial.println("Waveshare RTK Server " APP_VERSION);		// while(!Serial);

	_sdFile.Setup(); // Setup the SD card
	
	_lvCore.Setup(); // Initialize LVGL and the display
	Serial.println("Setup done");

	// Create the main screen (Use lv_scr_act())
	ui_MainScreen_screen_init();

	// Create the GPS status page
	_systemPageGps.Create(UIPageGroupPanel); // Create the GPS status page and add
	_swipePageIO.Create(UIPageGroupPanel); // Create the GPS status page and add
	_swipePageSettings.Create(UIPageGroupPanel); // Create the settings page and add

	// Fix the startup scroll offset error
	lv_obj_scroll_to_view(_systemPageGps.GetPanel(), LV_ANIM_OFF);

	_swipePageIO.RefreshData();
}

///////////////////////////////////////////////////////////////////////////////
// This function is called repeatedly in a loop
// It handles the LVGL tasks and allows the GUI to update
void loop()
{
	lv_task_handler(); // Let the GUI do its work
					   //	_panelLabelValue1.SetText(("Time: " + std::to_string(millis())).c_str());
					   //	_panelLabelValue2.SetText(("Time: " + std::to_string(millis())).c_str());
					   //	_panelLabelValue3.SetText(("Time: " + std::to_string(millis())).c_str());
}
