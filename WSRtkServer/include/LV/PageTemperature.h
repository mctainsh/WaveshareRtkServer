#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "LVCore.h"
// #include "History.h"

class PageTemperature;
PageTemperature *_pageTemperature = nullptr;

extern History _history;

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class PageTemperature : public SwipePageBase
{
private:
public:
	void Show()
	{
		// int pntCount = 200; // Number of points in the chart
		lv_obj_t *scr = lv_obj_create(NULL);

		// Create title and background
		CreatePanel(scr, LV_SYMBOL_BARS " Temperature (°C)", 0xF49595);

		// Create a chart
		lv_obj_t *chart = lv_chart_create(_uiPanelPage);
		lv_obj_set_size(chart, lv_pct(100), lv_pct(100));
		lv_obj_center(chart);
		lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/
		lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);

		// Create and configure a scale for the X axis
		// lv_scale_t * x_scale = lv_chart_get_scale(chart, LV_CHART_AXIS_PRIMARY_Y);
		// lv_scale_set_tick_length(x_scale, 10, 5);  // Major and minor tick lengths
		// lv_scale_set_tick_count(x_scale, 6, 1);    // Number of ticks and minor ticks between
		// lv_scale_set_labels(x_scale, "0\n10\n20\n30\n40\n50");  // Label text
		// lv_scale_t * x_scale = lv_chart_get_scale(chart, LV_CHART_AXIS_PRIMARY_Y);

		// lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, maj10, 10, 3, 3, true, 40);
		// lv_chart_set_axis_label(chart, LV_CHART_AXIS_PRIMARY_X, "0\n10\n20\n30\n40\n50");
		// lv_chart_set_axis_label(chart, LV_CHART_AXIS_PRIMARY_Y, "Y-Axis Label");
		// int TEMP_HISTORY_SIZE = 24 * 60;
		lv_chart_set_point_count(chart, TEMP_HISTORY_SIZE); // Set the number of points in the chart

		// Add data series
		lv_chart_series_t *ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

		auto temps = _history.GetTemperatures();
		for (uint32_t i = 0; i < TEMP_HISTORY_SIZE; i++)
		{
			lv_chart_set_next_value(chart, ser1, (int32_t)temps[i]);
			//lv_chart_set_next_value(chart, ser1, (int32_t)lv_rand(50, 90));
		}

		lv_chart_refresh(chart);		// Required after direct set
		lv_obj_set_flex_grow(chart, 1); // This makes it expand and push the next item down

		// Bottom button
		lv_obj_t *btnWrap = CreateFancyButton(LV_SYMBOL_CLOSE " Close", _uiPanelPage, PageTemperature::OnClose, lv_pct(100));
	
		// Animate the screen load
		lv_screen_load_anim(scr, lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OVER_LEFT, 300, 0, false);
		_pageTemperature = this;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Event handler for the button
	static void OnClose(lv_event_t *e)
	{
		if (lv_event_get_code(e) != LV_EVENT_CLICKED)
			return;
		if (_pageTemperature != nullptr)
			delete _pageTemperature;
		_pageTemperature = nullptr; // Clear the pointer to the PageTemperature instance
		lv_screen_load_anim(_lvCore.GetHomeScreen(), lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OUT_RIGHT, 300, 0, false);
	}
};
