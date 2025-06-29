#pragma once

// Using LVGL with Arduino requires some extra steps
// Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html

#include <lvgl.h>
#include <esp32-hal-ledc.h>
#include <Arduino_GFX_Library.h>
#include "TCA9554.h"
#include "esp_lcd_touch_axs15231b.h"
#include <Wire.h>

#define GFX_BL 6			// default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#define BACKLIGHT_CHANNEL 0 // Use channel 0 for backlight control

#define LCD_QSPI_CS 12
#define LCD_QSPI_CLK 5
#define LCD_QSPI_D0 1
#define LCD_QSPI_D1 2
#define LCD_QSPI_D2 3
#define LCD_QSPI_D3 4

#define I2C_SDA 8
#define I2C_SCL 7

#define W 320
#define H 480

#define DIRECT_RENDER_MODE // Uncomment to enable full frame buffer

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
uint32_t millis_cb(void);
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char *buf)
{
	LV_UNUSED(level);
	Serial.println(buf);
	Serial.flush();
}
#endif

static void sw_event_cbX(lv_event_t *e)
{
	lv_obj_t *sw = lv_event_get_target_obj(e);
	lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
	Serial.printf("SW %s %d\n", lv_label_get_text(label), lv_obj_has_state(sw, LV_STATE_CHECKED));
}

lv_obj_t *_lblBright;

///////////////////////////////////////////////////////////////////////////////
// Time functions
// WARNING : This class is called by logger so do not log yourself
class LVCore
{
private:
	TCA9554 TCA;	// Channel port expander?
	Arduino_DataBus *const _bus;
	Arduino_GFX *const _gfx;
	uint32_t _srcW;	// Screen width. Don't know why this is not W
	uint32_t _srcH;// .. same for height
	lv_display_t *_disp = nullptr;		   // Primary display
	lv_color_t *_disp_draw_buf1 = nullptr; // Memory buffer for display
	lv_color_t *_disp_draw_buf2 = nullptr; // ..

public:
	Arduino_GFX *GetGfx() { return _gfx; }

	LVCore() : TCA(0x20),
			   _bus(new Arduino_ESP32QSPI(LCD_QSPI_CS, LCD_QSPI_CLK, LCD_QSPI_D0, LCD_QSPI_D1, LCD_QSPI_D2, LCD_QSPI_D3)),
			   _gfx(new Arduino_AXS15231B(_bus, -1 /* RST */, 0 /* rotation */, false, W, H))

	{
		//	Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_QSPI_CS, LCD_QSPI_CLK, LCD_QSPI_D0, LCD_QSPI_D1, LCD_QSPI_D2, LCD_QSPI_D3);

		//	_gfx = new Arduino_AXS15231B(bus, -1 /* RST */, 0 /* rotation */, false, 320, 480);
		// _srcW = 0;
		// _srcH = 0;
		// bufSize = 0;
		// disp
		// disp_draw_buf1 = nullptr;
		// disp_draw_buf2 = nullptr;
	}

	///////////////////////////////////////////////////////////////////////////
	// Setup the LVGL controls
	// @return true if setup is successful, false otherwise
	bool Setup()
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
		bsp_touch_init(&Wire, -1, 0, W, H);
		Serial.begin(115200);
		// Serial.setDebugOutput(true);
		// while(!Serial);
		Serial.println("Waveshare RTK Server " APP_VERSION);
		String LVGL_Arduino = String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
		Serial.println(LVGL_Arduino);

		// Init Display
		if (!_gfx->begin())
		{
			Serial.println("gfx->begin() failed!");
		}
		_gfx->fillScreen(RGB565_BLUE);

#ifdef GFX_BL
		pinMode(GFX_BL, OUTPUT);
		digitalWrite(GFX_BL, HIGH);
		ledcAttachPin(GFX_BL, BACKLIGHT_CHANNEL); // Attach the pin to channel 0
		ledcSetup(BACKLIGHT_CHANNEL, 5000, 8);	  // Set channel 0 to 5kHz, 8-bit resolution
		ledcWrite(BACKLIGHT_CHANNEL, 255);		  // 100% brightness
#endif

		lv_init();

		// Set a tick source so that LVGL will know how much time elapsed.
		lv_tick_set_cb(millis_cb);

		// Register print function for debugging
#if LV_USE_LOG != 0
		lv_log_register_print_cb(my_print);
#endif

		_srcW = _gfx->width();
		_srcH = _gfx->height();

#ifdef DIRECT_RENDER_MODE
		uint32_t bufSize = _srcW * _srcH;
		_disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		_disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
		uint32_t bufSize = _srcW * 40;
		_disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
		_disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#endif
		if (!_disp_draw_buf1 || !_disp_draw_buf2)
		{
			Serial.println("LVGL disp_draw_buf allocate failed!");
			return false;
		}

		_disp = lv_display_create(_srcW, _srcH);
		lv_display_set_flush_cb(_disp, my_disp_flush);
#ifdef DIRECT_RENDER_MODE
		lv_display_set_buffers(_disp, _disp_draw_buf1, _disp_draw_buf2, bufSize * 2, LV_DISPLAY_RENDER_MODE_FULL);
#else
		lv_display_set_buffers(_disp, _disp_draw_buf1, _disp_draw_buf2, bufSize * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

		// Initialize the (dummy) input device driver
		lv_indev_t *indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); // Touchpad should have POINTER type
		lv_indev_set_read_cb(indev, my_touchpad_read);

		/* Option 1: Create a simple label
		 * ---------------------
		 */
		lv_obj_t *label = lv_label_create(lv_scr_act());
		lv_label_set_text(label, "JRM 1.6 LVGL(V" GFX_STR(LVGL_VERSION_MAJOR) "." GFX_STR(LVGL_VERSION_MINOR) "." GFX_STR(LVGL_VERSION_PATCH) ")");
		lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

		// Top switch
		lv_obj_t *sw = lv_switch_create(lv_scr_act());
		lv_obj_align(sw, LV_ALIGN_TOP_MID, 0, 50);
		lv_obj_add_state(sw, LV_STATE_CHECKED);
		lv_obj_add_event_cb(sw, sw_event_cbX, LV_EVENT_VALUE_CHANGED, label);

		// Slider
		lv_obj_t *slider = lv_slider_create(lv_scr_act());
		lv_slider_set_value(slider, 75, LV_ANIM_ON);
		lv_obj_set_width(slider, 200); /*Set the width*/
		// lv_obj_center(slider);														/*Align to the center of the parent (screen)*/
		lv_obj_align_to(slider, label, LV_ALIGN_OUT_TOP_MID, 0, -15);
		lv_obj_add_event_cb(slider, LVCore::OnSlider, LV_EVENT_VALUE_CHANGED, NULL); /*Assign an event function*/
		// lv_obj_add_event_cb(slider, std::bind(&LVCore::OnSlider,this), LV_EVENT_VALUE_CHANGED, NULL); /*Assign an event function*/
		// lv_obj_add_event_cb(slider, [this](lv_event_t* e) {OnSlider(e);}, LV_EVENT_VALUE_CHANGED, NULL); /*Assign an event function*/

		/*Create a label above the slider*/
		_lblBright = lv_label_create(lv_screen_active());
		lv_label_set_text(_lblBright, "0");
		lv_obj_align_to(_lblBright, slider, LV_ALIGN_OUT_TOP_MID, 0, -85); /*Align top of the slider*/
		return true;
	}

	//////////////////////////////////////////////////////////////////
	// Brightness LV_USE_SLIDER
	static void OnSlider(lv_event_t *e)
	{
		auto *self = static_cast<LVCore *>(lv_event_get_user_data(e));
		self->OnSliderEvent(e);
	}
	void OnSliderEvent(lv_event_t *e)
	{
		lv_obj_t *slider = lv_event_get_target_obj(e);
		int32_t v = lv_slider_get_value(slider);
		lv_label_set_text_fmt(_lblBright, "%" LV_PRId32, v);
		lv_obj_align_to(_lblBright, slider, LV_ALIGN_OUT_TOP_MID, v, -55);
		ledcWrite(BACKLIGHT_CHANNEL, std::max(1, (int)(v * 255 / 100)));
	}
};
extern LVCore _lvCore;

///////////////////////////////////////////////////////////////////////////////
// LVGL calls it when a rendered image needs to copied to the display
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
	uint32_t w = lv_area_get_width(area);
	uint32_t h = lv_area_get_height(area);

	_lvCore.GetGfx()->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

	// Call it to tell LVGL you are ready
	lv_disp_flush_ready(disp);
}

///////////////////////////////////////////////////////////////////////////////
// This function is called by LVGL to get the current time in milliseconds
// It is used to calculate the time elapsed since the last call
uint32_t millis_cb(void)
{
	return millis();
}

///////////////////////////////////////////////////////////////////////////////
// Read the touchpad
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
