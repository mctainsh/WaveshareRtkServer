#pragma once

#include "lvgl.h"

///////////////////////////////////////////////////////////////////////////////
// Panel containing a label and a value
class PanelLabelValue
{
private:
	lv_obj_t *ui_PanelGroup = NULL;
	lv_obj_t *ui_PanelLabel = NULL;
	lv_obj_t *ui_PanelTextArea = NULL;

public:
	void Create(lv_obj_t *parent, const char *label, const char *value)
	{
		// Single item panel group
		ui_PanelGroup = lv_obj_create(parent);
		lv_obj_set_width(ui_PanelGroup, lv_pct(100));
		lv_obj_set_height(ui_PanelGroup, 26);
		lv_obj_set_x(ui_PanelGroup, 0);
		lv_obj_set_y(ui_PanelGroup, 0);
		lv_obj_set_align(ui_PanelGroup, LV_ALIGN_CENTER);
		lv_obj_remove_flag(ui_PanelGroup, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_style_bg_color(ui_PanelGroup, lv_color_hex(0xF81717), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(ui_PanelGroup, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		// No padding
		lv_obj_set_style_pad_left(ui_PanelGroup, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_right(ui_PanelGroup, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_top(ui_PanelGroup, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_bottom(ui_PanelGroup, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		// No border
		lv_obj_set_style_border_color(ui_PanelGroup, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_opa(ui_PanelGroup, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		// No background color
		lv_obj_set_style_bg_color(ui_PanelGroup, lv_color_hex(0xFFFFFF), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(ui_PanelGroup, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

		// Label for the panel
		ui_PanelLabel = lv_label_create(ui_PanelGroup);
		lv_obj_set_width(ui_PanelLabel, LV_SIZE_CONTENT);
		lv_obj_set_height(ui_PanelLabel, LV_SIZE_CONTENT);
		lv_obj_set_align(ui_PanelLabel, LV_ALIGN_LEFT_MID);
		lv_label_set_long_mode(ui_PanelLabel, LV_LABEL_LONG_DOT);
		lv_label_set_text(ui_PanelLabel, label);
		lv_obj_remove_flag(ui_PanelLabel, LV_OBJ_FLAG_SCROLLABLE);

		// Text area for input
		// ui_PanelTextArea = lv_textarea_create(ui_PanelGroup);
		// lv_obj_set_width(ui_PanelTextArea, 200);
		// lv_obj_set_height(ui_PanelTextArea, LV_SIZE_CONTENT);
		// lv_obj_set_x(ui_PanelTextArea, 10);
		// lv_obj_set_y(ui_PanelTextArea, 0);
		// lv_obj_set_align(ui_PanelTextArea, LV_ALIGN_RIGHT_MID);
		// lv_textarea_set_text(ui_PanelTextArea, "some ZZZZthsf dsfh skjhf ksjdhf kjah ");
		// //lv_textarea_set_placeholder_text(ui_PanelTextArea, "Placeholder...");
		// lv_textarea_set_one_line(ui_PanelTextArea, true);

		ui_PanelTextArea = lv_textarea_create(ui_PanelGroup);
		lv_obj_set_width(ui_PanelTextArea, 216);
		lv_obj_set_height(ui_PanelTextArea, LV_SIZE_CONTENT); /// 20
		lv_obj_set_x(ui_PanelTextArea, 10);
		lv_obj_set_y(ui_PanelTextArea, 0);
		lv_obj_set_align(ui_PanelTextArea, LV_ALIGN_RIGHT_MID);
		lv_textarea_set_text(ui_PanelTextArea, value);
		// lv_textarea_set_placeholder_text(ui_PanelTextArea, "Placeholder...");
		lv_textarea_set_one_line(ui_PanelTextArea, true);
		//  lv_obj_remove_flag(ui_PanelTextArea,
		//                     LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
		//                     LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);     /// Flags
		lv_obj_set_scrollbar_mode(ui_PanelTextArea, LV_SCROLLBAR_MODE_OFF);

		// Thin padding
		lv_obj_set_style_pad_left(ui_PanelTextArea, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_right(ui_PanelTextArea, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_top(ui_PanelTextArea, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_bottom(ui_PanelTextArea, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
	}

	void SetText(const char *text)
	{
		if (ui_PanelTextArea)
			lv_textarea_set_text(ui_PanelTextArea, text);
	}
};
