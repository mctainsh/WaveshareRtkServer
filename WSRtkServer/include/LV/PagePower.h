#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "LVCore.h"

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class PagePower : public SwipePageBase
{
private:
public:
	/////////////////////////////////////////////////////////////////////////////////
	// Constructor
	PagePower()
	{
		_screen = lv_obj_create(NULL);

		// Create title and background
		CreatePanel(_screen, LV_SYMBOL_BATTERY_2 " Power Management", 0xbaffc9);

		// Create a table
		CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels

		AppendRowTitle("Battery", TblFormat::Highlight);
		AppendRowTitle("Charging");
		AppendRowTitle("Discharging");
		AppendRowTitle("Standby");
		AppendRowTitle("Chg. Status");
		AppendRowTitle("Percent", TblFormat::Right);

		AppendRowTitle("VBus", TblFormat::Highlight);
		AppendRowTitle("Input");
		AppendRowTitle("Good");

		AppendRowTitle("Voltages", TblFormat::Highlight);
		AppendRowTitle("VBus", TblFormat::Right);
		AppendRowTitle("Battery", TblFormat::Right);
		AppendRowTitle("System", TblFormat::Right);

		AppendRowTitle("Other", TblFormat::Highlight);
		AppendRowTitle("CPU Temp", TblFormat::Right);

		// Bottom button
		AddCloseButton(_screen, SwipePageBase::OnClose, this); // Add a close button to the bottom of the page
	}
};
