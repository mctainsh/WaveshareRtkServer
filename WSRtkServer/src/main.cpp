#include <Arduino.h>
#include "Global.h"
#include "LV/LVCore.h"
#include "UI/screens/ui_MainScreen.h"
#include <LV/PanelLabelValue.h>
#include "UI/screens/ui_GpsStatus.h"



LVCore _lvCore; // Create an instance of LVCore
PanelLabelValue _panelLabelValue1; // Create an instance of PanelLabelValue
PanelLabelValue _panelLabelValue2; // Create an instance of PanelLabelValue
PanelLabelValue _panelLabelValue3; // Create an instance of PanelLabelValue

///////////////////////////////////////////////////////////////////////////////
// This function is called once at the beginning of the program
// It initializes the display, touch, and LVGL library
void setup()
{
	_lvCore.Setup(); // Initialize LVGL and the display
	Serial.println("Setup done");

	//lv_example_scroll_2(); // Call the example function to create a scrollable panel

	// Create the main screen (Use lv_scr_act())
	ui_MainScreen_screen_init();

_panelLabelValue1.Create(ui_PanelPage, "TTime", "00:00:00"); // Create a panel with label and value
_panelLabelValue2.Create(ui_PanelPage, "Other", "00:00:00"); // Create a panel with label and value
_panelLabelValue3.Create(ui_PanelPage, "Tdsf kdhj sdhf dhf shf Time", "00:00:00"); // Create a panel with label and value
	//_panelLabelValue.Create(ui_PanelPage); // Create a panel with label and value


}

///////////////////////////////////////////////////////////////////////////////
// This function is called repeatedly in a loop
// It handles the LVGL tasks and allows the GUI to update
void loop()
{
	lv_task_handler(); // Let the GUI do its work 
	_panelLabelValue1.SetText(("Time: " + std::to_string(millis())).c_str()); 
//	_panelLabelValue2.SetText(("Time: " + std::to_string(millis())).c_str()); 
//	_panelLabelValue3.SetText(("Time: " + std::to_string(millis())).c_str()); 
}
