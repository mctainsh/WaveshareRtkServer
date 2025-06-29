#include <Arduino.h>
#include "Global.h"
#include "LV/LVCore.h"
#include <LV/screens/ui_Screen1.h>


LVCore _lvCore; // Create an instance of LVCore


static void sw_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * sw = lv_event_get_target_obj(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * list = (lv_obj_t *) lv_event_get_user_data(e);

        if(lv_obj_has_state(sw, LV_STATE_CHECKED)) lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLL_ONE);
        else lv_obj_remove_flag(list, LV_OBJ_FLAG_SCROLL_ONE);
    }
}

/**
 * Show an example to scroll snap
 */
void lv_example_scroll_2(void)
{
	// Panel with scroll snap
    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_set_size(panel, lv_pct(100), 120);
    lv_obj_set_scroll_snap_x(panel, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);
    lv_obj_align(panel, LV_ALIGN_BOTTOM_MID, 0, 0);

	// Add the buttons to the panel
    for(uint32_t i = 0; i < 10; i++) 
	{
        lv_obj_t * btn = lv_button_create(panel);
        lv_obj_set_size(btn, 310, lv_pct(100));

        lv_obj_t * label = lv_label_create(btn);
        if(i == 3) 
		{
			// Cannot stop pon this panel
            lv_label_set_text_fmt(label, "Panel %" LV_PRIu32"\nno snap", i);
            lv_obj_remove_flag(btn, LV_OBJ_FLAG_SNAPPABLE);
        }
        else 
		{
			// Can stop on this panel
            lv_label_set_text_fmt(label, "Panel %" LV_PRIu32, i);
        }

        lv_obj_center(label);
    }
    lv_obj_update_snap(panel, LV_ANIM_ON);

#if LV_USE_SWITCH
    /*Switch between "One scroll" and "Normal scroll" mode*/
    lv_obj_t * sw = lv_switch_create(lv_screen_active());
    lv_obj_align(sw, LV_ALIGN_TOP_RIGHT, -20, 10);
    lv_obj_add_event_cb(sw, sw_event_cb, LV_EVENT_ALL, panel);
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "One scroll");
    lv_obj_align_to(label, sw, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
#endif
}



///////////////////////////////////////////////////////////////////////////////
// This function is called once at the beginning of the program
// It initializes the display, touch, and LVGL library
void setup()
{
	_lvCore.Setup(); // Initialize LVGL and the display
	Serial.println("Setup done");

	lv_example_scroll_2(); // Call the example function to create a scrollable panel

	ui_Screen1_screen_init(lv_screen_active());
}

///////////////////////////////////////////////////////////////////////////////
// This function is called repeatedly in a loop
// It handles the LVGL tasks and allows the GUI to update
void loop()
{
	lv_task_handler(); // Let the GUI do its work 
	delay(50);
}
