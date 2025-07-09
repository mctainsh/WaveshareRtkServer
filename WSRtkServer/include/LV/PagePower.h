#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "LVCore.h"

class PagePower;
PagePower *_pagePower = nullptr;

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class PagePower : public SwipePageBase
{
private:
public:
	void Show()
	{
		lv_obj_t *scr = lv_obj_create(NULL);

		// Create title and background
		CreatePanel(scr, LV_SYMBOL_BATTERY_2 " Power Management", 0xbaffc9);

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
		AddCloseButton(scr, PagePower::OnClose); // Add a close button to the bottom of the page

		// Animate the screen load
		lv_screen_load_anim(scr, lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OVER_LEFT, 300, 0, false);
		_pagePower = this;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Event handler for the button
	static void OnClose(lv_event_t *e)
	{
		if (lv_event_get_code(e) != LV_EVENT_CLICKED)
			return;
		if (_pagePower != nullptr)
			delete _pagePower;
		_pagePower = nullptr; // Clear the pointer to the PagePower instance
		lv_screen_load_anim(_lvCore.GetHomeScreen(), lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OUT_RIGHT, 300, 0, false);
	}
};
