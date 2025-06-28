#include <Arduino.h>
#include "Global.h"
#include "LV/LVCore.h"


LVCore _lvCore; // Create an instance of LVCore

///////////////////////////////////////////////////////////////////////////////
// This function is called once at the beginning of the program
// It initializes the display, touch, and LVGL library
void setup()
{
	_lvCore.Setup(); // Initialize LVGL and the display
	Serial.println("Setup done");
}

void loop()
{
	lv_task_handler(); // Let the GUI do its work 
	delay(50);
}
