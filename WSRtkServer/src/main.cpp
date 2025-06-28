#include <Arduino.h>
/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */
#include <lvgl.h>

#include <esp32-hal-ledc.h>

/*To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 *You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 *Note that the `lv_examples` library is for LVGL v7 and you shouldn't install it for this version (since LVGL v8)
 *as the examples and demos are now part of the main LVGL library. */

// #include <examples/lv_examples.h>
// #include <demos/lv_demos.h>

#define DIRECT_RENDER_MODE	// Uncomment to enable full frame buffer


#include <Arduino_GFX_Library.h>

#include "TCA9554.h"

#include "esp_lcd_touch_axs15231b.h"
#include <Wire.h>

#define GFX_BL 6  // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

#define LCD_QSPI_CS 12
#define LCD_QSPI_CLK 5
#define LCD_QSPI_D0 1
#define LCD_QSPI_D1 2
#define LCD_QSPI_D2 3
#define LCD_QSPI_D3 4

#define I2C_SDA 8
#define I2C_SCL 7

TCA9554 TCA(0x20);

Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_QSPI_CS, LCD_QSPI_CLK, LCD_QSPI_D0, LCD_QSPI_D1, LCD_QSPI_D2, LCD_QSPI_D3);

Arduino_GFX *gfx = new Arduino_AXS15231B(bus, -1 /* RST */, 0 /* rotation */, false, 320, 480);
//Arduino_GFX *gfx = new Arduino_AXS15231B(bus, -1 /* RST */, 1/* rotation */, false, 480, 480);



uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_display_t *disp;
lv_color_t *disp_draw_buf1;
lv_color_t *disp_draw_buf2;

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char *buf)
{
	LV_UNUSED(level);
	Serial.println(buf);
	Serial.flush();
}
#endif

uint32_t millis_cb(void)
{
	return millis();
}

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
	uint32_t w = lv_area_get_width(area);
	uint32_t h = lv_area_get_height(area);

	gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

	/*Call it to tell LVGL you are ready*/
	lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
	touch_data_t touch_data;
	bsp_touch_read();

	if (bsp_touch_get_coordinates(&touch_data))
	{
		data->state = LV_INDEV_STATE_PR;
		/*Set the coordinates*/
		data->point.x = touch_data.coords[0].x;
		data->point.y = touch_data.coords[0].y;
	}
	else
	{
		data->state = LV_INDEV_STATE_REL;
	}
}

static void sw_event_cb(lv_event_t *e)
{
	lv_obj_t *sw = lv_event_get_target_obj(e);
	lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
	Serial.printf("SW %s %d\n", lv_label_get_text(label), lv_obj_has_state(sw, LV_STATE_CHECKED));
}

#define BACKLIGHT_CHANNEL 0 // Use channel 0 for backlight control
lv_obj_t *_lblBright;
//////////////////////////////////////////////////////////////////
// Brightness LV_USE_SLIDER
static void slider_event_cb(lv_event_t *e)
{
	lv_obj_t *slider = lv_event_get_target_obj(e);
	//lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);

	/*Refresh the text*/
	int32_t v = lv_slider_get_value(slider);
	lv_label_set_text_fmt(_lblBright, "%" LV_PRId32, v);
	lv_obj_align_to(_lblBright, slider, LV_ALIGN_OUT_TOP_MID, v, -15); /*Align top of the slider*/
	ledcWrite(BACKLIGHT_CHANNEL, std::max(1, (int)(v * 255 / 100)));  // Set duty cycle
//	ledcWrite(GFX_BL, std::max(1, (int)(v * 255 / 100)));  // Set duty cycle
}

///////////////////////////////////////////////////////////////////////////////
// This function is called once at the beginning of the program
// It initializes the display, touch, and LVGL library
void setup()
{
#ifdef DEV_DEVICE_INIT
	DEV_DEVICE_INIT();
#endif
	Wire.begin(I2C_SDA, I2C_SCL);
	TCA.begin();
	TCA.pinMode1(1, OUTPUT);
	TCA.write1(1, 1);
	delay(10);
	TCA.write1(1, 0);
	delay(10);
	TCA.write1(1, 1);
	delay(200);
	bsp_touch_init(&Wire, -1, 0, 320, 480);
	Serial.begin(115200);
	// Serial.setDebugOutput(true);
	// while(!Serial);
	Serial.println("JRM_GFX LVGL_Arduino_v9 example ");
	String LVGL_Arduino = String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
	Serial.println(LVGL_Arduino);


	//	lv_display_set_rotation(NULL, LV_DISPLAY_ROTATION_0);




	// Init Display
	if (!gfx->begin())
	{
		Serial.println("gfx->begin() failed!");
	}
	gfx->fillScreen(RGB565_BLUE);

#ifdef GFX_BL
	pinMode(GFX_BL, OUTPUT);
	digitalWrite(GFX_BL, HIGH);
	ledcAttachPin(GFX_BL, BACKLIGHT_CHANNEL);		// Attach the pin to channel 0
	ledcSetup(BACKLIGHT_CHANNEL, 5000, 8);			// Set channel 0 to 5kHz, 8-bit resolution
	ledcWrite(BACKLIGHT_CHANNEL, 255);  			// 100% brightness 
#endif

	lv_init();

	/*Set a tick source so that LVGL will know how much time elapsed. */
	lv_tick_set_cb(millis_cb);

	/* register print function for debugging */
#if LV_USE_LOG != 0
	lv_log_register_print_cb(my_print);
#endif

	screenWidth = gfx->width();
	screenHeight = gfx->height();


#ifdef DIRECT_RENDER_MODE
	bufSize = screenWidth * screenHeight;
	disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
	bufSize = screenWidth * 40;
	disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#endif
	if (!disp_draw_buf1 || !disp_draw_buf2)
	{
		Serial.println("LVGL disp_draw_buf allocate failed!");
	}
	else
	{
		disp = lv_display_create(screenWidth, screenHeight);
		lv_display_set_flush_cb(disp, my_disp_flush);
#ifdef DIRECT_RENDER_MODE
		lv_display_set_buffers(disp, disp_draw_buf1, disp_draw_buf2, bufSize * 2, LV_DISPLAY_RENDER_MODE_FULL);
#else
		lv_display_set_buffers(disp, disp_draw_buf1, disp_draw_buf2, bufSize * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

		/*Initialize the (dummy) input device driver*/
		lv_indev_t *indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
		lv_indev_set_read_cb(indev, my_touchpad_read);

		/* Option 1: Create a simple label
     * ---------------------
     */
		lv_obj_t *label = lv_label_create(lv_scr_act());
		lv_label_set_text(label, "JRM 1.3 LVGL(V" GFX_STR(LVGL_VERSION_MAJOR) "." GFX_STR(LVGL_VERSION_MINOR) "." GFX_STR(LVGL_VERSION_PATCH) ")");
		lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

		// Top switch
		lv_obj_t *sw = lv_switch_create(lv_scr_act());
		lv_obj_align(sw, LV_ALIGN_TOP_MID, 0, 50);
		lv_obj_add_state(sw, LV_STATE_CHECKED);
		lv_obj_add_event_cb(sw, sw_event_cb, LV_EVENT_VALUE_CHANGED, label);

		// Bottom switch
		sw = lv_switch_create(lv_scr_act());
		lv_obj_align(sw, LV_ALIGN_BOTTOM_MID, 0, -50);
		lv_obj_add_state(sw, LV_STATE_CHECKED);
		lv_obj_add_event_cb(sw, sw_event_cb, LV_EVENT_VALUE_CHANGED, label);


		// Slider
		lv_obj_t *slider = lv_slider_create(lv_screen_active());
		lv_slider_set_value(slider, 75, LV_ANIM_ON);
		lv_obj_set_width(slider, 200); /*Set the width*/
		//lv_obj_center(slider);														/*Align to the center of the parent (screen)*/
		lv_obj_align_to(slider, label, LV_ALIGN_OUT_TOP_MID, 0, -15);
		lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL); /*Assign an event function*/

		/*Create a label above the slider*/
		_lblBright = lv_label_create(lv_screen_active());
		lv_label_set_text(_lblBright, "0");
		lv_obj_align_to(_lblBright, slider, LV_ALIGN_OUT_TOP_MID, 0, -85); /*Align top of the slider*/


		/* Option 2: Try an example. See all the examples
     *  - Online: https://docs.lvgl.io/master/examples.html
     *  - Source codes: https://github.com/lvgl/lvgl/tree/master/examples
     * ----------------------------------------------------------------
     */
		// lv_example_btn_1();

		/* Option 3: Or try out a demo. Don't forget to enable the demos in lv_conf.h. E.g. LV_USE_DEMOS_WIDGETS
     * -------------------------------------------------------------------------------------------
     */
		// lv_demo_widgets();
		// lv_demo_benchmark();
		// lv_demo_keypad_encoder();
		// lv_demo_music();
		// lv_demo_stress();
	}

	Serial.println("Setup done");
}

void loop()
{
	lv_task_handler(); /* let the GUI do its work */
	delay(50);
}
