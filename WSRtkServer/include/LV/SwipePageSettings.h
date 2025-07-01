#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"
#include <SDFile.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <Global.h>

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
		CreatePanel(parentGroupPanel, "Settings", 0x00FFFF);

		_panelVersion.Create(_uiPanelPage, "Firmware", APP_VERSION); // Create a panel with label and value

		// Slider label
		lv_obj_t *label = lv_label_create(_uiPanelPage);
		lv_obj_set_width(label, LV_SIZE_CONTENT);
		lv_obj_set_height(label, LV_SIZE_CONTENT);
		lv_obj_set_align(label, LV_ALIGN_TOP_LEFT);
		lv_label_set_text(label, "Brightness");

		// Slider
		lv_obj_t *slider = lv_slider_create(_uiPanelPage);
		lv_slider_set_value(slider, 100, LV_ANIM_ON);
		lv_obj_set_width(slider, lv_pct(100));
		//lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 30);
		// lv_obj_align_to(slider, label, LV_ALIGN_TOP_MID, 0, 30);
		lv_obj_add_event_cb(slider, SwipePageSettings::OnSlider, LV_EVENT_VALUE_CHANGED, NULL); /*Assign an event function*/
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
		ledcWrite(BACKLIGHT_CHANNEL, std::max(1, (int)(v * 255 / 100)));
	}
};
