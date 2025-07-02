#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"

///////////////////////////////////////////////////////////////////////////////
// This is a page with a number of controls on it is added to a scrolling panel
class SwipePageGps : public SwipePageBase
{
private:
	PanelLabelValue _panelLabelValue1; // Create an instance of PanelLabelValue
	PanelLabelValue _panelLabelValue2; // Create an instance of PanelLabelValue
	PanelLabelValue _panelLabelValue3; // Create an instance of PanelLabelValue

public:
	void Create(lv_obj_t *parentGroupPanel)
	{
		CreatePanel(parentGroupPanel, "GPS Status", 0xFF00FF);							   // Create the panel for this page
		_panelLabelValue1.Create(_uiPanelPage, "TTime", "00:00:00");					   // Create a panel with label and value
		_panelLabelValue2.Create(_uiPanelPage, "Other", "00:00:00");					   // Create a panel with label and value
		_panelLabelValue3.Create(_uiPanelPage, "Tdsf kdhj sdhf dhf shf Time", "00:00:00"); // Create a panel with label and value

		lv_obj_t *table = lv_table_create(_uiPanelPage);
		lv_obj_set_height(table, 200);
		lv_obj_set_width(table, lv_pct(100)); // Set the width to 100% of the parent panel
		//lv_obj_set_x(table, -15);
		// lv_obj_set_y(table, 0);
		lv_obj_set_align(table, LV_ALIGN_TOP_MID);

		// Table padding and border
		lv_obj_set_style_pad_top(table, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_bottom(table, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_left(table, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_right(table, 2, LV_PART_ITEMS | LV_STATE_DEFAULT);

		// Setup right alignment for the second column
		lv_table_set_col_width(table, 0, 100); // Set the width
		lv_table_set_col_width(table, 1, 212); // Set the width

		/*Fill the first column*/
		lv_table_set_cell_value(table, 0, 0, "Name ");
		lv_table_set_cell_value(table, 1, 0, "Apple ");
		lv_table_set_cell_value(table, 2, 0, "Banana ");
		lv_table_set_cell_value(table, 3, 0, "Lemon ");
		lv_table_set_cell_value(table, 4, 0, "Grape ");
		lv_table_set_cell_value(table, 5, 0, "Melon ");
		lv_table_set_cell_value(table, 6, 0, "Peach ");
		lv_table_set_cell_value(table, 7, 0, "Nuts ");

		/*Fill the second column*/
		lv_table_set_cell_value(table, 0, 1, "Price");
		lv_table_set_cell_value(table, 1, 1, "$7");
		lv_table_set_cell_value(table, 3, 1, "$6");
		lv_table_set_cell_value(table, 4, 1, "$2sdf kalksd lakjsd lkaj slkd j aslk jdlkajs dlkasj ldkja s jdalk");
		lv_table_set_cell_user_data(table, 4, 1, (void *)0x1234);

		lv_table_set_cell_value(table, 5, 1, "$5");
	
		lv_table_set_cell_value(table, 6, 1, "$1");
		lv_table_set_cell_value(table, 7, 1, "$9");


		// Setup daw event callback for the table
		lv_obj_add_event_cb(table, OnTableDrawEvent, LV_EVENT_DRAW_TASK_ADDED, NULL);
		lv_obj_add_flag(table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
	}
};
