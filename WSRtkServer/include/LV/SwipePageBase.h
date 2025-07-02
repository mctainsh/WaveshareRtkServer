#pragma once

#include "lvgl.h"


static void OnTableDrawEvent(lv_event_t *e)
{
	lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
	lv_draw_dsc_base_t *base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
	
	// If the cells are drawn...
	if (base_dsc->part == LV_PART_ITEMS)
	{
		uint32_t row = base_dsc->id1;
		uint32_t col = base_dsc->id2;

		// Make the texts in the first Row centered
		if (row == 0)
		{
			lv_draw_label_dsc_t *label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
			if (label_draw_dsc)
			{
				label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
			}
			lv_draw_fill_dsc_t *fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
			if (fill_draw_dsc)
			{
				fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), fill_draw_dsc->color, LV_OPA_20);
				fill_draw_dsc->opa = LV_OPA_COVER;
			}
		}
		
		// In the first column align the texts to the right
		else if (col == 0)
		{
			lv_draw_label_dsc_t *label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
			if (label_draw_dsc)
			{
				label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
			}
		}

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
		void* cellData = lv_table_get_cell_user_data(base_dsc->obj, row, col);
		if (cellData)
		{
			// Example: If the cell data is a specific value, change the style
			if (cellData == (void *)0x1234) // Replace with your condition
			{
				lv_draw_label_dsc_t *label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
				if (label_draw_dsc)
				{
					label_draw_dsc->color = lv_color_hex(0xFF0000); // Change text color to red
				}
			}
		}	

	}
}

///////////////////////////////////////////////////////////////////////////////
// This is the base page all swiping pages are derived from
class SwipePageBase
{
private:
protected:
	lv_obj_t *_uiPanelPage;

	///////////////////////////////////////////////////////////////////////////
	// Create the panel all items are added to
	void CreatePanel(lv_obj_t *parentGroupPanel, const char *title, u32_t bgColor)
	{
		// Panel around the page
		_uiPanelPage = lv_obj_create(parentGroupPanel);
		lv_obj_set_height(_uiPanelPage, lv_pct(100));
		lv_obj_set_width(_uiPanelPage, 320);
		//   lv_obj_set_x(ui_PanelGroup, -16);
		// lv_obj_set_y(_uiPanelPage, 0);
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
		lv_obj_t *titleLabel = lv_label_create(_uiPanelPage);
		lv_obj_set_width(titleLabel, LV_SIZE_CONTENT);
		lv_obj_set_height(titleLabel, LV_SIZE_CONTENT);
		lv_obj_set_align(titleLabel, LV_ALIGN_TOP_LEFT);
		lv_label_set_text(titleLabel, title);
		//lv_obj_set_style_text_decor(ui_TitleLabel, LV_TEXT_DECOR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
	}

public:
	lv_obj_t *GetPanel() { return _uiPanelPage;	}
};
