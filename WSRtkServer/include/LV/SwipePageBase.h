#pragma once

#include "lvgl.h"

extern lv_font_t FontAwesomeRegular18;

// Enum for table format options
enum TblFormat
{
	None = 0,
	Right = 2,
	Centre = 4,
	Bold = 8,
	Highlight = 16,
	SingleLine = 32, // Single line text
};

static void OnTableDrawEvent(lv_event_t *e);

///////////////////////////////////////////////////////////////////////////////
// This is the base page all swiping pages are derived from
class SwipePageBase
{
public:
	bool Ready() const { return _ready && _screen != nullptr; } // True if the page is ready to be updated
	lv_obj_t *GetPanel() { return _uiPanelPage; }

	///////////////////////////////////////////////////////////////////////////
	// Show the page. Called with pages that are derived from this class
	void Show()
	{
		lv_screen_load_anim(_screen, lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OVER_LEFT, 300, 0, false);
		_ready = true; // Set the page as ready to be updated
	}

	///////////////////////////////////////////////////////////////////////////
	// Set value in the table
	void SetTableValue(uint32_t row, const char *value, int column = 1)
	{
		// Check if the table is hidden before setting the value
		if (_table && !lv_obj_has_flag(_table, LV_OBJ_FLAG_HIDDEN))
			lv_table_set_cell_value(_table, row, column, value);
		else
			Serial.printf("Table is hidden, cannot set value for row %p: %s\n", _table, value);
	}
	void SetTableString(uint32_t row, const std::string &value, int column = 1)
	{
		SetTableValue(row, value.c_str(), column);
	}
	void SetTableValue(uint32_t row, int64_t value, int column = 1)
	{
		SetTableValue(row, ToThousands(value).c_str(), column);
	}
	void SetTableShort(uint32_t row, int64_t value, int column = 1)
	{
		SetTableString(row, ShortenNumber(value), column);
	}

private:
protected:
	lv_obj_t *_uiPanelPage; // Panel we are drawing on
	lv_obj_t *_titleLabel;
	lv_obj_t *_table;	 // Table if we have one
	u32_t _rowCount = 0; // Number of rows in the table
	const char *_title;

	lv_obj_t *_screen = nullptr; // The screen we are drawing on
	bool _ready = false;		 // True if the page is ready to be updated

protected:
	///////////////////////////////////////////////////////////////////////////
	// Create the panel all items are added to
	void CreatePanel(lv_obj_t *parentGroupPanel, const char *title, u32_t bgColor)
	{
		// Panel around the page
		_uiPanelPage = lv_obj_create(parentGroupPanel);
		lv_obj_set_height(_uiPanelPage, lv_pct(100));
		lv_obj_set_width(_uiPanelPage, 320);
		lv_obj_set_align(_uiPanelPage, LV_ALIGN_BOTTOM_MID);
		lv_obj_set_flex_flow(_uiPanelPage, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(_uiPanelPage, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
		lv_obj_remove_flag(_uiPanelPage, LV_OBJ_FLAG_SCROLLABLE);

		// No padding
		lv_obj_set_style_pad_left(_uiPanelPage, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_right(_uiPanelPage, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_top(_uiPanelPage, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_bottom(_uiPanelPage, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		// No padding between rows and columns
		lv_obj_set_style_pad_row(_uiPanelPage, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_column(_uiPanelPage, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		// Soft background color
		lv_obj_set_style_bg_color(_uiPanelPage, lv_color_hex(bgColor), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(_uiPanelPage, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

		// Add title label
		_titleLabel = lv_label_create(_uiPanelPage);
		lv_obj_set_width(_titleLabel, LV_SIZE_CONTENT);
		lv_obj_set_height(_titleLabel, LV_SIZE_CONTENT);
		lv_obj_set_align(_titleLabel, LV_ALIGN_TOP_LEFT);
		lv_label_set_text(_titleLabel, title);
		lv_obj_set_style_text_font(_titleLabel, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
	}

	///////////////////////////////////////////////////////////////////////////
	// Create a table
	lv_obj_t *CreateTable(lv_obj_t *parent, int32_t height)
	{
		_table = lv_table_create(parent);
		lv_obj_set_height(_table, height);
		lv_obj_set_width(_table, lv_pct(100));
		lv_obj_set_align(_table, LV_ALIGN_TOP_MID);

		// Table padding and border
		lv_obj_set_style_pad_top(_table, 3, LV_PART_ITEMS | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_bottom(_table, 3, LV_PART_ITEMS | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_left(_table, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_right(_table, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);

		// Setup right alignment for the second column
		lv_table_set_col_width(_table, 0, 110); // Set the width
		lv_table_set_col_width(_table, 1, 202); // Set the width

		// Setup draw event callback for the table
		lv_obj_add_event_cb(_table, OnTableDrawEvent, LV_EVENT_DRAW_TASK_ADDED, NULL);
		lv_obj_add_flag(_table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

		// Disable table highlighting
		lv_obj_clear_flag(_table, LV_OBJ_FLAG_CLICKABLE); // Optional: disables click
		lv_obj_clear_state(_table, LV_STATE_FOCUSED);	  // Remove focus state
		return _table;
	}

	///////////////////////////////////////////////////////////////////////////
	// Append a row title to the table
	int AppendRowTitle(const char *title, int format = (int)TblFormat::None, int columns = 1)
	{
		lv_table_set_cell_value(_table, _rowCount, 0, title);
		for (int n = 1; n <= columns; n++)
			lv_table_set_cell_value(_table, _rowCount, n, "");
		// lv_table_set_cell_value(_table, _rowCount, 1, "");
		if (format)
		{
			// First column only get highlight
			if (format & TblFormat::Highlight)
				lv_table_set_cell_user_data(_table, _rowCount, 0, (void *)TblFormat::Highlight);

			// Remaining columns
			for (int n = 1; n <= columns; n++)
			{
				lv_table_set_cell_user_data(_table, _rowCount, n, (void *)format);
				if (format & TblFormat::SingleLine)
					lv_table_set_cell_ctrl(_table, _rowCount, n, LV_TABLE_CELL_CTRL_TEXT_CROP);
			}
		}
		_rowCount++;
		return _rowCount - 1; // Return the row index
	}

	///////////////////////////////////////////////////////////////////////////
	// This function is called when the close button is clicked
	static lv_obj_t *CreateFancyButton(const char *title, lv_obj_t *parent, lv_event_cb_t event_cb, void *user_data, int32_t width, int32_t height = 60)
	{
		lv_obj_t *wrap = LVCore::ClearPanel(parent, 6, 6, 6, 6);
		lv_obj_set_height(wrap, height);
		lv_obj_set_width(wrap, width);

		CreateFancyButtonOnly(title, wrap, event_cb, user_data);
		return wrap;
	}

	///////////////////////////////////////////////////////////////////////////
	// This function is called when the close button is clicked
	static lv_obj_t *CreateFancyButtonOnly(const char *title, lv_obj_t *parent, lv_event_cb_t event_cb, void *user_data)
	{
		// Create the button
		lv_obj_t *btn = lv_button_create(parent);
		lv_obj_remove_style_all(btn); // Remove the style coming from the theme
		lv_obj_add_style(btn, &StyleFancyButton, 0);
		lv_obj_add_style(btn, &StyleFancyButtonPressed, LV_STATE_PRESSED);
		lv_obj_set_size(btn, lv_pct(100), lv_pct(100));
		lv_obj_center(btn);

		// Add event callback
		lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, user_data);

		// Label
		lv_obj_t *btnLabel = lv_label_create(btn);
		lv_label_set_text(btnLabel, title);
		lv_obj_set_style_text_font(btnLabel, &FontAwesomeRegular18, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_center(btnLabel);
		return btn;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Event handler for the close button
	static void OnClose(lv_event_t *e)
	{
		if (lv_event_get_code(e) != LV_EVENT_CLICKED)
			return;

		auto *self = static_cast<SwipePageBase *>(lv_event_get_user_data(e));
		if (self == nullptr)
		{
			Logln("E917 - OnClose called with null self");
			return;
		}
		self->_ready = false; // Set the page as not ready

		// Animate to home
		lv_screen_load_anim(_lvCore.GetHomeScreen(), lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OUT_RIGHT, 300, 0, false);
	}

	///////////////////////////////////////////////////////////////////////////
	// Add a close button to the bottom of the page
	void AddCloseButton(lv_obj_t *screen, lv_event_cb_t event_cb, void *user_data)
	{
		// Spacer to push next item to bottom
		lv_obj_t *spacer = lv_obj_create(_uiPanelPage);
		lv_obj_set_size(spacer, LV_PCT(100), 1);								 // Width 100%, height minimal
		lv_obj_set_style_bg_opa(spacer, LV_OPA_TRANSP, 0);						 // Make it invisible
		lv_obj_set_style_border_opa(spacer, 0, LV_PART_MAIN | LV_STATE_DEFAULT); // No border
		lv_obj_set_flex_grow(spacer, 1);										 // This makes it expand and push the next item down

		// Add close button to bottom of the page
		CreateFancyButton(LV_SYMBOL_CLOSE " Close", _uiPanelPage, event_cb, user_data, lv_pct(100));
	}
};

///////////////////////////////////////////////////////////////////////////////
// This function is called when the table cell is drawn
static void OnTableDrawEvent(lv_event_t *e)
{
	lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
	if (!draw_task)
		return;
	lv_draw_dsc_base_t *base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
	if (!base_dsc)
		return;

	// If the cells are drawn...
	if (base_dsc->part != LV_PART_ITEMS)
		return;

	uint32_t row = base_dsc->id1;
	uint32_t col = base_dsc->id2;

	// Make the texts in the first Row centered
	// if (row == 0)
	// {
	// 	lv_draw_label_dsc_t *label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
	// 	if (label_draw_dsc)
	// 	{
	// 		label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
	// 	}
	// 	lv_draw_fill_dsc_t *fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
	// 	if (fill_draw_dsc)
	// 	{
	// 		fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), fill_draw_dsc->color, LV_OPA_20);
	// 		fill_draw_dsc->opa = LV_OPA_COVER;
	// 	}
	// }

	// In the first column align the texts to the right
	// if (col == 0)
	// {
	// 	lv_draw_label_dsc_t *label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
	// 	if (label_draw_dsc)
	// 	{
	// 		label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
	// 	}
	// }

	// Make every 2nd row grayish
	if ((row != 0 && row % 2) == 0)
	{
		lv_draw_fill_dsc_t *fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
		if (fill_draw_dsc)
		{
			fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), fill_draw_dsc->color, LV_OPA_10);
			fill_draw_dsc->opa = LV_OPA_COVER;
		}
	}

	// Apply specific styles based on cell data
	void *cellData = lv_table_get_cell_user_data(base_dsc->obj, row, col);
	if (cellData)
	{
		TblFormat format = static_cast<TblFormat>(reinterpret_cast<uintptr_t>(cellData));
		lv_draw_label_dsc_t *label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
		if (label_draw_dsc)
		{
			if (format & TblFormat::Bold)
				label_draw_dsc->color = lv_color_hex(0x0000FF); // Change text color to red
			if (format & TblFormat::Right)
				label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
			if (format & TblFormat::Centre)
				label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
		}

		lv_draw_fill_dsc_t *fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
		if (fill_draw_dsc && (format & TblFormat::Highlight))
		{
			fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), fill_draw_dsc->color, LV_OPA_20);
			fill_draw_dsc->opa = LV_OPA_COVER;
		}
	}
}
