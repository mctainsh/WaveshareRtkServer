#pragma once

// Using LVGL with Arduino requires some extra steps
// Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html

// See lv_conf.h for configuration options (.pio\libdeps\Waveshare-S3-35\lv_conf.h)


///////////////////////////////////////////////////////////////////////////////
// Using Custom Fonts
// This is a custom font for FontAwesome Regular 18pt
// Generated using the lv_font_conv tool
// 		https://lvgl.io/tools/fontconverter
//   Name    : FontAwesomeRegular18
//   Size    : 18
//   Style   : Regular
//   Bpp     : 1
//   Fallback: lv_font_montserrat_18
//   Font    : Font Awesome 6 Free-Solid-900.otf
//   Range : 0xf2fe,0xF2C9,0xf7c0,0xf7bf
// Generate
//   Place in the fonts folder
//   Direct to #include "lvgl.h"



#include <lvgl.h>
#include <esp32-hal-ledc.h>
#include <Arduino_GFX_Library.h>
#include "TCA9554.h"
#include "esp_lcd_touch_axs15231b.h"
#include <Wire.h>
//#include "FontAwesomeRegular18.h"

#define GFX_BL 6 // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

#define LCD_QSPI_CS 12
#define LCD_QSPI_CLK 5
#define LCD_QSPI_D0 1
#define LCD_QSPI_D1 2
#define LCD_QSPI_D2 3
#define LCD_QSPI_D3 4

#define W 320
#define H 480

#define DIRECT_RENDER_MODE // Uncomment to enable full frame buffer

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
uint32_t millis_cb(void);
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);
// lv_obj_t *MakeStatusButton(lv_obj_t *parent, const char *title, lv_event_cb_t event_cb);
// lv_obj_t *ClearPanel(lv_obj_t *parent, int32_t l, int32_t r, int32_t t, int32_t b);
//  void SetIndicatorColour(lv_obj_t *btn, StatusButtonState state);

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char *buf)
{
	LV_UNUSED(level);
	Serial.println(buf);
	Serial.flush();
}
#endif

static lv_style_t StyleFancyButton;
static lv_style_t StyleFancyButtonPressed;

extern lv_font_t FontAwesomeRegular18;

///////////////////////////////////////////////////////////////////////////////
// Time functions
// WARNING : This class is called by logger so do not log yourself
class LVCore
{
private:
	TCA9554 TCA; // Channel port expander?
	Arduino_DataBus *const _bus;
	Arduino_GFX *const _gfx;
	uint32_t _srcW;						   // Screen width. Don't know why this is not W
	uint32_t _srcH;						   // .. same for height
	lv_display_t *_disp = nullptr;		   // Primary display
	lv_color_t *_disp_draw_buf1 = nullptr; // Memory buffer for display
	lv_color_t *_disp_draw_buf2 = nullptr; // ..
	lv_obj_t *_homeScreen = nullptr;	   // Home screen object
	lv_obj_t *_labelBattery = nullptr;	   // Label Battery percentage
	lv_obj_t *_labelTime = nullptr;		   // Label for the time in top row

	lv_obj_t *_btnStatusWifi = nullptr; // Button for WiFi status
	lv_obj_t *_btnStatusGps = nullptr;	// Button for GPS connection status
	lv_obj_t *_btnStatusSvr1 = nullptr; // Button for NTRIP server connection status
	lv_obj_t *_btnStatusSvr2 = nullptr; // ..
	lv_obj_t *_btnStatusSvr3 = nullptr; // ..

public:
	Arduino_GFX *GetGfx() { return _gfx; }
	lv_obj_t *GetHomeScreen() { return _homeScreen; }

	lv_obj_t *_label = nullptr; // Label for the top left corner

	LVCore() : TCA(0x20),
			   _bus(new Arduino_ESP32QSPI(LCD_QSPI_CS, LCD_QSPI_CLK, LCD_QSPI_D0, LCD_QSPI_D1, LCD_QSPI_D2, LCD_QSPI_D3)),
			   _gfx(new Arduino_AXS15231B(_bus, -1 /* RST */, 0 /* rotation */, false, W, H))
	{
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

		Logf("LVGL:%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());

		// Init Display
		if (!_gfx->begin())
			Logf("\t*** ERROR : gfx->begin() failed!");
		_gfx->fillScreen(RGB565_BLUE);

		Logln("\tSetup backlight");
#ifdef GFX_BL
		pinMode(GFX_BL, OUTPUT);
		digitalWrite(GFX_BL, HIGH);
		ledcSetup(BACKLIGHT_CHANNEL, 5000, 8);	  // 1) Set channel 0 to 5kHz, 8-bit resolution
		ledcAttachPin(GFX_BL, BACKLIGHT_CHANNEL); // 2) Attach the pin to channel 0
		ledcWrite(BACKLIGHT_CHANNEL, 255);		  // 3) 100% brightness
#endif

		// Initialize the LVGL library
		lv_init();

		// Set a tick source so that LVGL will know how much time elapsed.
		lv_tick_set_cb(millis_cb);

		// Register print function for debugging
#if LV_USE_LOG != 0
		lv_log_register_print_cb(my_print);
#endif

		_srcW = _gfx->width();
		_srcH = _gfx->height();

		Logln("\tBuffers");

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

		Logf("\tDisplay %dx%d", _srcW, _srcH);
		_disp = lv_display_create(_srcW, _srcH);
		lv_display_set_flush_cb(_disp, my_disp_flush);
#ifdef DIRECT_RENDER_MODE
		lv_display_set_buffers(_disp, _disp_draw_buf1, _disp_draw_buf2, bufSize * 2, LV_DISPLAY_RENDER_MODE_FULL);
#else
		lv_display_set_buffers(_disp, _disp_draw_buf1, _disp_draw_buf2, bufSize * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

		Logln("\tMake button style");
		MakeButtonStyle();

		// Initialize the (dummy) input device driver
		Logln("\tEnable touch");
		lv_indev_t *indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); // Touchpad should have POINTER type
		lv_indev_set_read_cb(indev, my_touchpad_read);

		// Black background
		lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(lv_scr_act(), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
		_homeScreen = lv_scr_act(); // Save the home screen

		// Setup battery labels
		_labelBattery = lv_label_create(lv_scr_act());
		lv_label_set_text(_labelBattery, "Battery");
		lv_obj_align(_labelBattery, LV_ALIGN_TOP_LEFT, 0, 5);
		lv_obj_set_style_text_color(_labelBattery, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

		// Time label
		_labelTime = lv_label_create(lv_scr_act());
		lv_label_set_text(_labelTime, "Time");
		lv_obj_align(_labelTime, LV_ALIGN_TOP_LEFT, 0, 25);
		lv_obj_set_style_text_color(_labelTime, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT);

		// Add a clear panel for the status buttons
		lv_obj_t *statusPanel = ClearPanel(lv_scr_act(), 0, 0, 0, 0);
		lv_obj_set_width(statusPanel, LV_SIZE_CONTENT);
		lv_obj_align(statusPanel, LV_ALIGN_TOP_RIGHT, 0, 0);
		lv_obj_set_flex_flow(statusPanel, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(statusPanel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
		lv_obj_remove_flag(statusPanel, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_style_pad_column(statusPanel, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

		_btnStatusWifi = MakeStatusButton(
			statusPanel, LV_SYMBOL_WIFI, [](lv_event_t *e)
			{
			// Handle battery button click
			Serial.println("WIFI button clicked"); });
		_btnStatusGps = MakeStatusButton(statusPanel, FA_SATELLITE_DISH, [](lv_event_t *e)
										 {
			// Handle battery button click
			Serial.println("GPS button clicked"); });

		_btnStatusSvr1 = MakeStatusButton(statusPanel, "S1", [](lv_event_t *e)
										  {
			// Handle battery button click
			Serial.println("S1 button clicked"); });
		_btnStatusSvr2 = MakeStatusButton(statusPanel, "S2", [](lv_event_t *e)
										  {
			// Handle battery button click
			Serial.println("Battery S2 button clicked"); });
		_btnStatusSvr3 = MakeStatusButton(statusPanel, "S3", [](lv_event_t *e)
										  {
			// Handle battery button click
			Serial.println("Battery S3 button clicked"); });

		return true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Set the status button background and foreground colour
	void SetIndicatorColour(lv_obj_t *btn, ConnectionState state)
	{
		if (btn == nullptr)
			return;
		lv_color_t fg;
		lv_color_t bg;
		switch (state)
		{
		case ConnectionState::Unknown:
			fg = lv_color_hex(0x0);
			bg = lv_color_hex(0xFFFF00);
			break;
		case ConnectionState::Disconnected:
			fg = lv_color_hex(0xFFFFFF);
			bg = lv_color_hex(0xFF0000);
			break;
		case ConnectionState::Connected:
			fg = lv_color_hex(0x0);
			bg = lv_color_hex(0x00FF00);
			break;
		case ConnectionState::Disabled:
			fg = lv_color_hex(0x505050);
			bg = lv_color_hex(0xE0E0E0);
			break;
		}
		lv_obj_set_style_bg_color(btn, bg, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(btn, fg, LV_PART_MAIN | LV_STATE_DEFAULT);
	}

	///////////////////////////////////////////////////////////////////////////
	// Show the WiFi connection state as a colour indicator
	void UpdateWiFiIndicator()
	{
		SetIndicatorColour(_btnStatusWifi, WiFi.status() == WL_CONNECTED ? ConnectionState::Connected : ConnectionState::Disconnected);
	}
	void SetGpsConnected(ConnectionState s)
	{
		SetIndicatorColour(_btnStatusGps, s);
	}
	void SetNtripConnected(ConnectionState s1, ConnectionState s2, ConnectionState s3)
	{
		SetIndicatorColour(_btnStatusSvr1, s1);
		SetIndicatorColour(_btnStatusSvr2, s2);
		SetIndicatorColour(_btnStatusSvr3, s3);
	}

	void UpdateStatusButtons()
	{
		// Update the status buttons based on the current state
		if (_btnStatusWifi)
			lv_obj_clear_state(_btnStatusWifi, LV_STATE_CHECKED);
		if (_btnStatusGps)
			lv_obj_clear_state(_btnStatusGps, LV_STATE_CHECKED);
		if (_btnStatusSvr1)
			lv_obj_clear_state(_btnStatusSvr1, LV_STATE_CHECKED);
		if (_btnStatusSvr2)
			lv_obj_clear_state(_btnStatusSvr2, LV_STATE_CHECKED);
		if (_btnStatusSvr3)
			lv_obj_clear_state(_btnStatusSvr3, LV_STATE_CHECKED);
	}

	///////////////////////////////////////////////////////////////////////////
	// Set the title time label
	void SetTitleTime(const std::string &time)
	{
		if (_labelTime)
			lv_label_set_text(_labelTime, time.c_str());
		// lv_obj_invalidate(_labelTime);
	}

	///////////////////////////////////////////////////////////////////////////
	// Set the battery percentage label
	void SetBatteryPercent(int percentage, bool charging)
	{
		if (!_labelBattery)
			return;
		std::string s;
		if (percentage < 0)
		{
			lv_label_set_text(_labelBattery, LV_SYMBOL_BATTERY_EMPTY);
			return;
		}
		if (percentage < 20)
			s = LV_SYMBOL_BATTERY_EMPTY;
		else if (percentage < 40)
			s = LV_SYMBOL_BATTERY_1;
		else if (percentage < 60)
			s = LV_SYMBOL_BATTERY_2;
		else if (percentage < 80)
			s = LV_SYMBOL_BATTERY_3;
		else
			s = LV_SYMBOL_BATTERY_FULL;
		if (charging)
			s += (" " LV_SYMBOL_CHARGE); // Add charging symbol if charging

		lv_label_set_text(_labelBattery, (s + " " + std::to_string(percentage) + "%").c_str());
	}

	///////////////////////////////////////////////////////////////////////////
	// Create a button style for the fancy button
	void MakeButtonStyle()
	{
		/*Init the style for the default state*/
		lv_style_init(&StyleFancyButton);

		lv_style_set_radius(&StyleFancyButton, 3);

		lv_style_set_bg_opa(&StyleFancyButton, LV_OPA_100);
		lv_style_set_bg_color(&StyleFancyButton, lv_palette_main(LV_PALETTE_BLUE));
		lv_style_set_bg_grad_color(&StyleFancyButton, lv_palette_darken(LV_PALETTE_BLUE, 2));
		lv_style_set_bg_grad_dir(&StyleFancyButton, LV_GRAD_DIR_VER);

		lv_style_set_border_opa(&StyleFancyButton, LV_OPA_40);
		lv_style_set_border_width(&StyleFancyButton, 2);
		lv_style_set_border_color(&StyleFancyButton, lv_palette_main(LV_PALETTE_GREY));

		const int shadow_width = 3;
		lv_style_set_shadow_width(&StyleFancyButton, shadow_width);
		lv_style_set_shadow_color(&StyleFancyButton, lv_palette_main(LV_PALETTE_GREY));
		lv_style_set_shadow_offset_x(&StyleFancyButton, shadow_width);
		lv_style_set_shadow_offset_y(&StyleFancyButton, shadow_width);

		lv_style_set_outline_opa(&StyleFancyButton, LV_OPA_COVER);
		lv_style_set_outline_color(&StyleFancyButton, lv_palette_main(LV_PALETTE_BLUE));

		lv_style_set_text_color(&StyleFancyButton, lv_color_white());
		lv_style_set_pad_all(&StyleFancyButton, 10);

		// Init the pressed style
		lv_style_init(&StyleFancyButtonPressed);

		// Add a large outline when pressed
		lv_style_set_outline_width(&StyleFancyButtonPressed, 30);
		lv_style_set_outline_opa(&StyleFancyButtonPressed, LV_OPA_TRANSP);

		lv_style_set_translate_y(&StyleFancyButtonPressed, 5);
		lv_style_set_shadow_offset_y(&StyleFancyButtonPressed, 3);
		lv_style_set_bg_color(&StyleFancyButtonPressed, lv_palette_darken(LV_PALETTE_BLUE, 2));
		lv_style_set_bg_grad_color(&StyleFancyButtonPressed, lv_palette_darken(LV_PALETTE_BLUE, 4));

		// Add a transition to the outline
		static lv_style_transition_dsc_t trans;
		static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
		lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

		lv_style_set_transition(&StyleFancyButtonPressed, &trans);
	}

	///////////////////////////////////////////////////////////////////////////////
	// Create a status button with a title and an event callback
	// This is used for the status buttons in the top right corner
	lv_obj_t *MakeStatusButton(lv_obj_t *parent, const char *title, lv_event_cb_t event_cb)
	{
		lv_obj_t *btn = lv_button_create(parent); /*Add a button the current screen*/
		lv_obj_set_size(btn, 40, 40);
		SetIndicatorColour(btn, ConnectionState::Unknown);
		//lv_obj_set_style_text_font(btn, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

		// Border and radius
		lv_obj_set_style_border_color(btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_radius(btn, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

		// Padding
		lv_obj_set_style_pad_left(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_right(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_top(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_bottom(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		// Text on the button
		lv_obj_t *label = lv_label_create(btn);
		lv_label_set_text(label, title);
		lv_obj_set_style_text_font(label, &FontAwesomeRegular18, LV_PART_MAIN | LV_STATE_DEFAULT);

		lv_obj_center(label);

		// Add event callback
		lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

		return btn;
	}

public:
	///////////////////////////////////////////////////////////////////////////
	// Create a clear panel for wrapping to add padding
	static lv_obj_t *ClearPanel(lv_obj_t *parent, int32_t l, int32_t r, int32_t t, int32_t b)
	{
		// Add a transparent panel around the button to give it padding
		lv_obj_t *wrap = lv_obj_create(parent);
		lv_obj_set_height(wrap, LV_SIZE_CONTENT);
		lv_obj_set_width(wrap, lv_pct(100));
		lv_obj_remove_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);

		// No background or border
		lv_obj_set_style_bg_opa(wrap, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_opa(wrap, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		// Padding
		lv_obj_set_style_pad_left(wrap, l, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_right(wrap, r, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_top(wrap, t, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_pad_bottom(wrap, b, LV_PART_MAIN | LV_STATE_DEFAULT);

		return wrap;
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