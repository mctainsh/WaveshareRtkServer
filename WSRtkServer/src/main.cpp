#include <Arduino.h>
#include "Global.h"
#include "LV/LVCore.h"
#include "UI/screens/ui_MainScreen.h"
#include <LV/SwipePageGps.h>
#include <LV/SwipePageIO.h>

#include "Hardware/SdFile.h"
#include <LV/SwipePageSettings.h>

#include <SPI.h> // Needed for time
#include <SensorPCF85063.hpp>

#include "Hardware/PowerManagementSystem.h"

#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <mDNS.h>		 // Setup *.local domain name

#include "HandyLog.h"
#include "HandyString.h"
#include "MyDisplay.h"
#include "GpsParser.h"
#include "NTRIPServer.h"
#include "Hardware/MyFiles.h"
#include <Web/WebPortal.h>
#include "WiFiEvents.h"
#include "History.h"

WiFiManager _wifiManager;

unsigned long _slowLoopWaitTime = 0; // Time of last 10 second
unsigned long _fastLoopWaitTime = 0; // Time of last second
int _loopPersSecondCount = 0;		 // Number of times the main loops runs in a second
unsigned long _lastButtonPress = 0;	 // Time of last button press to turn off display on T-Display-S3
History _history;					 // Temperature history

WebPortal _webPortal;

uint8_t _button1Current = HIGH; // Top button on left
uint8_t _button2Current = HIGH; // Bottom button when

MyFiles _myFiles;
MyDisplay _display;
GpsParser _gpsParser(_display);
NTRIPServer _ntripServer0(0);
NTRIPServer _ntripServer1(1);
NTRIPServer _ntripServer2(2);
std::string _baseLocation = "";				  // Lat long of the base
std::string _mdnsHostName;					  // Host name for mDNS
HandyTime _handyTime;						  // Time functions for NTP and local time
PowerManagementSystem _powerManagementSystem; // Power management system

// WiFi monitoring states
#define WIFI_STARTUP_TIMEOUT 90000
// unsigned long _wifiFullResetTime = 0;
wl_status_t _lastWifiStatus = wl_status_t::WL_NO_SHIELD;

// bool IsButtonReleased(uint8_t button, uint8_t *pCurrent);
// bool IsWifiConnected();

SensorPCF85063 _rtc; // Create an instance of the PCF85063 RTC

SDFile _sdFile; // Create an instance of SDFile
LVCore _lvCore;

// Pages
SwipePageGps _systemPageGps;
SwipePageGps _swipePageGps;
SwipePageIO _swipePageIO;
//SwipePagePower _swipePagePower;
SwipePageSettings _swipePageSettings;

extern PagePower *_pagePower;

///////////////////////////////////////////////////////////////////////////////
// This function is called once at the beginning of the program
// It initializes the display, touch, and LVGL library
void setup()
{
	Serial.begin(115200); // Initialize serial communication for debugging
	while (!Serial)
		;		// Wait for serial port to connect (uncomment if needed)
	delay(100); // Wait for a short time to ensure the serial connection is established
	Serial.setDebugOutput(true);
	Serial.println("Waveshare RTK Server " APP_VERSION); // while(!Serial);

	_sdFile.Setup(); // Setup the SD card

	// Initialize the QMI8658 IMU sensor
	// Try to initialize the RTC module using I2C with specified SDA and SCL pins
	if (!_rtc.begin(Wire, I2C_SDA, I2C_SCL))
	{
		Serial.println("Failed to find PCF85063 'Clock' - check your wiring!");
	// 	// Enter an infinite loop to halt the program
	// 	while (1)
	// 	{
	// 		Serial.println("Failed to find PCF85063 - check your wiring!");
	// 	}
	}

	// Set the defined date and time on the RTC
	//   rtc.setDateTime(year, month, day, hour, minute, second);

	if (!_rtc.isClockIntegrityGuaranteed())
	{
	 	Serial.println("[ERROR]:Clock integrity is not guaranteed; oscillator has stopped or has been interrupted");
	}

	_lvCore.Setup(); // Initialize LVGL and the display

	// Create the main screen (Use lv_scr_act())
	ui_MainScreen_screen_init();

	// Create the GPS status page
	_swipePageIO.Create(UIPageGroupPanel);		 // Create the GPS status page and add
	_swipePageSettings.Create(UIPageGroupPanel); // Create the settings page and add
	//_swipePagePower.Create(UIPageGroupPanel); 
	_systemPageGps.Create(UIPageGroupPanel);	 // Create the GPS status page and add

	// Fix the startup scroll offset error
	lv_obj_scroll_to_view(_swipePageSettings.GetPanel(), LV_ANIM_OFF);

	_swipePageIO.RefreshData();

	_powerManagementSystem.Setup(); // Setup the power management system

	Serial.println(" ========================== Setup done ========================== ");
}

u32_t _tick;

///////////////////////////////////////////////////////////////////////////////
// This function is called repeatedly in a loop
// It handles the LVGL tasks and allows the GUI to update
void loop()
{
	lv_task_handler(); // Let the GUI do its work

	if (millis() - _tick < 1000) // Update every second
		return;
	_tick = millis(); // Update the tick time

	// Check power management system
	_powerManagementSystem.PowerLoop();
	if( _pagePower != nullptr )
	{
		// Refresh the power page if it is open
		//_pagePower->RefreshData();
		_powerManagementSystem.RefreshData(_pagePower);
	}
//	_powerManagementSystem.RefreshData(_swipePagePower);
	//_swipePagePower.RefreshData(_powerManagementSystem.Get());



	 char buf[64];

	struct tm timeinfo;
	// // Get the time C library structure
	 _rtc.getDateTime(&timeinfo);

	size_t written = strftime(buf, 64, "%A, %B %d %Y %H:%M:%S", &timeinfo);

	if (written != 0)
	 {
	 	Serial.println(buf);
	 lv_label_set_text(_lvCore._label, buf);
	 }

	// written = strftime(buf, 64, "%b %d %Y %H:%M:%S", &timeinfo);
	// if (written != 0)
	//	Serial.println(buf);
}

//////////////////////////////////////////////////////////////////////////////
// Read the base location from the disk
void SaveBaseLocation(std::string newBaseLocation)
{
	_baseLocation = newBaseLocation;
	_myFiles.WriteFile(BASE_LOCATION_FILENAME, newBaseLocation.c_str());
}
