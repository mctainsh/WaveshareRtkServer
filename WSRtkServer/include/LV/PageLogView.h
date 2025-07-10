#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "LVCore.h"

class PageLogView;
PageLogView *_pageLogView = nullptr;

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class PageLogView : public SwipePageBase
{
private:
	lv_obj_t *_screen;
	int pageIndex = 0; // Index of the current page in the log
	int pageSize = 20; // Number of lines per page
	lv_obj_t *_logBox;

public:
	void Show()
	{
		_pageLogView = this;
		_screen = lv_obj_create(NULL);

		// Create title and background
		CreatePanel(_screen, LV_SYMBOL_LIST " Log View", 0xbaffc9);

		// Create a table
		CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels

		// Make text box olding the log
		_logBox = lv_textarea_create(_uiPanelPage);
		lv_obj_set_size(_logBox, lv_pct(100), lv_pct(100)); // Full width and height
		lv_obj_set_align(_logBox, LV_ALIGN_TOP_MID);		// Align to the top center
		// lv_obj_set_style_text_font(_logBox, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT); // Set font for the text area
		lv_obj_set_style_text_color(_logBox, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT); // Set text color to black
		lv_obj_set_style_bg_color(_logBox, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);		   // Set background color to white
		lv_obj_set_style_bg_opa(_logBox, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);			   // Set background opacity to cover
		lv_obj_set_style_border_opa(_logBox, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);		   // Make border transparent
		lv_obj_set_style_border_width(_logBox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);					   // No border
		lv_obj_set_style_pad_all(_logBox, 3, LV_PART_MAIN | LV_STATE_DEFAULT);						   // Padding around the text

		lv_textarea_set_one_line(_logBox, true);

		lv_obj_set_flex_grow(_logBox, 1); // This makes it expand and push the next item down

		// Load the main log into the text area
		SetLogText();

		// Add three bottons to the bottom of the page
lv_obj_t * cont = lv_obj_create(_uiPanelPage);
lv_obj_set_size(cont, 240, 60); // Adjust as needed
lv_obj_center(cont);

// Set flex layout: row direction, space evenly
lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

for (int i = 0; i < 3; ++i) {
    lv_obj_t * btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 60, 40); // Fixed size for each button

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text_fmt(label, "Btn %d", i + 1);
    lv_obj_center(label);
}


		// Add close button to bottom of the page
		CreateFancyButton(LV_SYMBOL_CLOSE " Close", _uiPanelPage, PageLogView::OnClose, this, lv_pct(100));

		// Animate the screen load
		lv_screen_load_anim(_screen, lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OVER_LEFT, 300, 0, false);
		_pageLogView = this;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Load the text for the new index
	void SetLogText()
	{
		std::string logText;
		std::vector<std::string> logLines = CopyMainLog();
		for (int n = pageIndex; n < logLines.size(); n++)
		{
			if (n >= pageIndex + pageSize)
				break;
			const auto &line = logLines[n];
			logText += line + "\n"; // Append each line with a newline character
		}
		lv_textarea_set_text(_logBox, logText.c_str()); // Set the text area content
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Event handler for the button
	static void OnClose(lv_event_t *e)
	{
		if (lv_event_get_code(e) != LV_EVENT_CLICKED)
			return;
		lv_screen_load_anim(_lvCore.GetHomeScreen(), lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OUT_RIGHT, 300, 0, true);
		if (_pageLogView != nullptr)
			delete _pageLogView;
		_pageLogView = nullptr; // Clear the pointer to the PageLogView instance
	}

	void ZZZPageLogView()
	{
		Logln("PageLogView destructor called");
		if (_screen != nullptr)
			lv_obj_del(_screen); // Delete the screen object
								 //		lv_obj_del(UIMainScreen); // Delete the main screen object
								 // Destructor to clean up resources if needed
								 //		_pageLogView = nullptr; // Clear the pointer to the PageLogView instance
	}
};
