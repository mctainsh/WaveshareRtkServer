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
	int pageSize = 40; // Number of lines per page
	lv_obj_t *_logBox;

public:
	void Show()
	{
		_pageLogView = this;
		_screen = lv_obj_create(NULL);

		// Create title and background
		CreatePanel(_screen, LV_SYMBOL_LIST " Log View", 0xbaffc9);

		// Create a table
		// CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels

		// Make text box olding the log
		_logBox = lv_textarea_create(_uiPanelPage);
		lv_obj_set_size(_logBox, lv_pct(100), lv_pct(100));
		lv_obj_set_align(_logBox, LV_ALIGN_LEFT_MID);
		lv_obj_set_style_text_font(_logBox, &lv_font_unscii_8, 0);
		lv_obj_set_style_text_color(_logBox, lv_color_hex(0x00FF00), 0);
		lv_obj_set_style_bg_color(_logBox, lv_color_black(), 0);
		lv_obj_set_style_bg_opa(_logBox, LV_OPA_COVER, 0);
		lv_obj_set_style_border_opa(_logBox, LV_OPA_TRANSP, 0);
		lv_obj_set_style_border_width(_logBox, 0, 0);
		lv_obj_set_style_pad_all(_logBox, 3, 0);

		lv_textarea_set_one_line(_logBox, true);
		lv_obj_set_flex_grow(_logBox, 1); // This makes it expand and push the next item down

		// Load the main log into the text area
		SetLogText();

		// Add three buttons to the bottom of the page

		lv_obj_t *cont = LVCore::ClearPanel(_uiPanelPage, 0, 0, 0, 0);

		//
		// lv_obj_t *cont = lv_obj_create(_uiPanelPage);
		lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT); // Adjust as needed
		lv_obj_center(cont);
		lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
		// lv_obj_set_style_bg_opa
		//		lv_obj_set_style_pad_left(cont, 0, 0);
		//		lv_obj_set_style_pad_right(cont, 0, 0);
		//		lv_obj_set_style_pad_top(cont, 0, 0);
		//		lv_obj_set_style_pad_bottom(cont, 0, 0);

		//		lv_obj_set_style_pad_all(cont, 0, 0);
		lv_obj_set_style_pad_column(cont, 0, 0);

		// Set flex layout: row direction, space evenly
		lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		// Page up and down
		auto *p = CreateFancyButtonOnly(LV_SYMBOL_UP " PgUp", cont, PageLogView::OnPageUp, this);
		lv_obj_set_size(p, lv_pct(30), 60);

		p = CreateFancyButtonOnly(LV_SYMBOL_DOWN " PgDn", cont, PageLogView::OnPageDown, this);
		lv_obj_set_size(p, lv_pct(30), 60);

		// Add close button to bottom of the page
		p = CreateFancyButtonOnly(LV_SYMBOL_CLOSE " Close", cont, PageLogView::OnClose, this);
		lv_obj_set_size(p, lv_pct(30), 60);

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

		// Apply limits
		if (logLines.size() < pageIndex + pageSize)
			pageIndex = logLines.size() - pageSize;
		if (pageIndex < 0)
			pageIndex = 0;

		// Read lines from the log starting at pageIndex
		for (int n = pageIndex; n < logLines.size(); n++)
		{
			if (n >= pageIndex + pageSize)
				break;
			const auto &line = logLines[n];
			logText += line + "\n";
		}

		// Set the text area content
		lv_textarea_set_text(_logBox, logText.c_str());
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

	/////////////////////////////////////////////////////////////////////////////////
	// Page up or down
	static void OnPageDown(lv_event_t *e)
	{
		OnPage(e, false); 
	}
	static void OnPageUp(lv_event_t *e)
	{
		OnPage(e, true); 
	}
	/////////////////////////////////////////////////////////////////////////////////
	// Page up or down
	static void OnPage(lv_event_t *e, bool up)
	{
		if (lv_event_get_code(e) != LV_EVENT_CLICKED)
			return;

		auto *self = static_cast<PageLogView *>(lv_event_get_user_data(e));
		if (self == nullptr)
		{
			Logln("E817 - OnPage called with null self");
			return;
		}
		if( up)
			self->pageIndex -= self->pageSize; // Move to the previous page
		else
			self->pageIndex += self->pageSize; // Move to the next page
		self->SetLogText();
	}
};
