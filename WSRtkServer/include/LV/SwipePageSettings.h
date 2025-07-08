#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"
#include "Hardware/SdFile.h"
#include <WiFi.h>
#include <SPIFFS.h>
#include <Global.h>
#include "PagePower.h"

extern SDFile _sdFile;
extern std::string _mdnsHostName;

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class SwipePageSettings : public SwipePageBase
{
private:
	PanelLabelValue _panelVersion;	
		
public:
	void Create(lv_obj_t *parentGroupPanel)
	{
		CreatePanel(parentGroupPanel, LV_SYMBOL_SETTINGS " Settings", 0xbae1ff);

		_panelVersion.Create(_uiPanelPage, "Firmware", APP_VERSION); // Create a panel with label and value

		// Slider label
		lv_obj_t *label = lv_label_create(_uiPanelPage);
		lv_obj_set_width(label, LV_SIZE_CONTENT);
		lv_obj_set_height(label, LV_SIZE_CONTENT);
		lv_obj_set_align(label, LV_ALIGN_TOP_LEFT);
		lv_label_set_text(label, "Brightness");

		lv_obj_t *wrap = LVCore::ClearPanel(_uiPanelPage, 6, 6, 2, 2);

		// Slider
		lv_obj_t *slider = lv_slider_create(wrap);
		lv_slider_set_value(slider, 100, LV_ANIM_ON);
		lv_obj_set_width(slider, lv_pct(80));
		lv_obj_center(slider);
		lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0);
		//lv_obj_align_to(slider, label, LV_ALIGN_TOP_MID, 0, 0);
		lv_obj_add_event_cb(slider, SwipePageSettings::OnSlider, LV_EVENT_VALUE_CHANGED, NULL); /*Assign an event function*/

		// Add buttons
		CreateFancyButton(LV_SYMBOL_BATTERY_2 " Power", _uiPanelPage, SwipePageSettings::OnPowerBtn, lv_pct(100));
		CreateFancyButton(LV_SYMBOL_DRIVE " System", _uiPanelPage, SwipePageSettings::OnDriveBtn, lv_pct(100));
	}

	void RefreshData()
	{
	}

	//////////////////////////////////////////////////////////////////
	// Brightness LV_USE_SLIDER
	static void OnSlider(lv_event_t *e)
	{
		auto *self = static_cast<SwipePageSettings *>(lv_event_get_user_data(e));
		self->OnSliderEvent(e);
	}
	void OnSliderEvent(lv_event_t *e)
	{
		lv_obj_t *slider = lv_event_get_target_obj(e);
		int32_t v = lv_slider_get_value(slider);
		// lv_label_set_text_fmt(_lblBright, "%" LV_PRId32, v);
		// lv_obj_align_to(_lblBright, slider, LV_ALIGN_OUT_TOP_MID, v, -55);
		// TODO : Make it more aggressive around 100%
		ledcWrite(BACKLIGHT_CHANNEL, std::max(1, (int)(v * 255 / 100)));
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Event handler for the power page
	static void OnPowerBtn(lv_event_t *e)
	{
		if ( lv_event_get_code(e) != LV_EVENT_CLICKED )
			return;
		auto *self = static_cast<SwipePageSettings *>(lv_event_get_user_data(e));
		self->OnPowerBtnClicked(e);
	}
	void OnPowerBtnClicked(lv_event_t *e)
	{
		_pagePower = new PagePower(); // Create a new instance of PagePower
		if( _pagePower == nullptr )
		{
			Serial.println("Failed to create PagePower instance");
			return;
		}
		_pagePower->Show();
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Event handler for the drive page
	static void OnDriveBtn(lv_event_t *e)
	{
		if ( lv_event_get_code(e) != LV_EVENT_CLICKED )
			return;
		auto *self = static_cast<SwipePageSettings *>(lv_event_get_user_data(e));
		self->OnDriveBtnClicked(e);
	}
	void OnDriveBtnClicked(lv_event_t *e)
	{
		_pageIO = new PageIO(); 
		if( _pageIO == nullptr )
		{
			Serial.println("Failed to create PageIO instance");
			return;
		}
		_pageIO->Show();
	}
};
