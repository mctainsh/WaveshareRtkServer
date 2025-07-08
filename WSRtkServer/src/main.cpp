#include <Arduino.h>
#include "Global.h"
#include "LV/LVCore.h"
#include "UI/screens/ui_MainScreen.h"
#include <LV/SwipePageGps.h>
#include <LV/SwipePageIO.h>

#include "Hardware/SdFile.h"
#include <LV/SwipePageSettings.h>

#include "Hardware/PowerManagementSystem.h"

#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include "HandyLog.h"
#include "HandyString.h"
#include "MyDisplay.h"
#include "GpsParser.h"
#include "NTRIPServer.h"
#include "Hardware/MyFiles.h"
#include <Web/WebPortal.hpp>
#include "WiFiEvents.h"
#include "History.h"

WiFiManager _wifiManager;

#define SLOW_TIMER 10000 // 10 seconds
#define FAST_TIMER 1000	 // 1 second

unsigned long _slowLoopWaitTime = SLOW_TIMER; // Time of last 10 second
unsigned long _fastLoopWaitTime = 0;		  // Time of last second
int _loopPersSecondCount = 0;				  // Number of times the main loops runs in a second
unsigned long _lastButtonPress = 0;			  // Time of last button press to turn off display on T-Display-S3
History _history;							  // Temperature history
int _drawLoopSkip = 0;						  // Draw every 100th draw loop to reduce CPU load

WebPortal _webPortal;

MyFiles _myFiles; // Flash files
SDFile _sdFile;	  // SD Card files
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
wl_status_t _lastWifiStatus = wl_status_t::WL_NO_SHIELD;

bool IsWifiConnected();
LVCore _lvCore;

// Pages
SwipePageGps _systemPageGps;
SwipePageGps _swipePageGps;
SwipePageIO _swipePageIO;
//  SwipePagePower _swipePagePower;
SwipePageSettings _swipePageSettings;

extern PagePower *_pagePower;

void FastLoop(unsigned long t, StatusButtonState gpsState);
void SlowLoop(unsigned long t);

///////////////////////////////////////////////////////////////////////////////
// This function is called once at the beginning of the program
// It initializes the display, touch, and LVGL library
void setup()
{
	//_wifiManager.resetSettings();

	Serial.begin(115200); // Initialize serial communication for debugging
	delay(100);			  // Wait for a short time to ensure the serial connection is established
	Serial.setDebugOutput(true);
	Serial.println("Waveshare RTK Server " APP_VERSION); // while(!Serial);

	SetupLog(); // Call this before any logging
	Logf("Starting %s. Cores:%d", APP_VERSION, configNUM_CORES);

	// Setup the serial buffer for the GPS port
	Logf("GPS Buffer size %d", Serial2.setRxBufferSize(GPS_BUFFER_SIZE));

	Logln("Enable WIFI");
	SetupWiFiEvents();

	// Check if the internal time chip is working
	_handyTime.Setup(); // Setup the time functions

	// Setup the SD Card
	_sdFile.Setup();

	// If we have time, start the logging
	if (_handyTime.IsTimeSet())
	{
		Logln("Start logging early");
		auto logCopy = CopyMainLog();
		_sdFile.StartLogFile(&logCopy);
	}

	// Verify file IO (This can take up tpo 60s is SPIFFs not initialised)
	Logln("Setup SPIFFS");
	// tft.println("This can take up to 60 seconds ...");
	_myFiles.Setup();
	_myFiles.LoadString(_baseLocation, BASE_LOCATION_FILENAME);
	_handyTime.LoadTimezoneOffset(_myFiles.LoadString(TIMEZONE_MINUTES));

	// Load the NTRIP server settings
	Logln("Setup NTRIP Connections");
	_ntripServer0.LoadSettings();
	_ntripServer1.LoadSettings();
	_ntripServer2.LoadSettings();
	_gpsParser.Setup(&_ntripServer0, &_ntripServer1, &_ntripServer2);

	// Setup host name to have RTK_ prefix
	WiFi.setHostname(MakeHostName().c_str());

	// Initialize LVGL and the display
	_lvCore.Setup();

	// Create the main screen (Use lv_scr_act())
	ui_MainScreen_screen_init();

	// Create the GPS status page
	_systemPageGps.Create(UIPageGroupPanel);	 // Create the GPS status page and add
	_swipePageIO.Create(UIPageGroupPanel);		 // Create the GPS status page and add
	_swipePageSettings.Create(UIPageGroupPanel); // Create the settings page and add

	// Fix the startup scroll offset error
	lv_obj_scroll_to_view(_swipePageIO.GetPanel(), LV_ANIM_OFF);

	_swipePageIO.RefreshData();
	_lvCore.UpdateWiFiIndicator();

	_powerManagementSystem.Setup(); // Setup the power management system

	Serial.println(" ========================== Setup done ========================== ");
	_webPortal.Setup();
}

///////////////////////////////////////////////////////////////////////////////
// This function is called repeatedly in a loop
// It handles the LVGL tasks and allows the GUI to update
void loop()
{
	// Let the LVGL library handle its tasks
	if (_drawLoopSkip++ > 1000)
	{
		_drawLoopSkip = 0;
		lv_task_handler();
	}

	// Check for new data GPS serial data
	StatusButtonState gpsState = StatusButtonState::Unknown;
	if (IsWifiConnected())
		gpsState = _gpsParser.ReadDataFromSerial(Serial2);

	// Trigger something 1 every seconds
	int t = millis();
	_loopPersSecondCount++;
	if ((t - _fastLoopWaitTime) > 1000)
		FastLoop(t, gpsState);

	// Run every 10 seconds
	if ((t - _slowLoopWaitTime) > SLOW_TIMER)
		SlowLoop(t);

	// Check the web portal
	_webPortal.Loop();
}

///////////////////////////////////////////////////////////////////////////////
// This function is called every second to refresh the display and handle power management
	StatusButtonState gpsState = StatusButtonState::Unknown;
void FastLoop(unsigned long t, StatusButtonState gpsState)
{
	// Refresh RTK Display
	for (int i = 0; i < RTK_SERVERS; i++)
		_display.RefreshRtk(i);
	_fastLoopWaitTime = t;
	_loopPersSecondCount = 0;
	_lvCore.SetTitleTime(_handyTime.Format("%a %H:%M:%S"));
	//_lvCore.SetTitleDate(_handyTime.Format("%d %b %Y"));

	// Check power management system
	_powerManagementSystem.PowerLoop();
	if (_pagePower != nullptr)
		_powerManagementSystem.RefreshData(_pagePower);

	// If WiFI disconnected refresh the WiFi status
	if (WiFi.status() != WL_CONNECTED)
	{
		_swipePageIO.RefreshData();
		_lvCore.UpdateWiFiIndicator();
	}
	_lvCore.SetGpsConnected(gpsState);
}

///////////////////////////////////////////////////////////////////////////////
// This function is called every 10 seconds to handle slow tasks
void SlowLoop(unsigned long t)
{
	_swipePageIO.RefreshData();
	_lvCore.UpdateWiFiIndicator();

	// Check memory pressure
	auto free = ESP.getFreeHeap();
	auto total = ESP.getHeapSize();

	auto temperature = _history.CheckTemperatureLoop();

	// Update the loop performance counter
	Serial.printf("%s Loop %d G:%ld Heap:%d%% %.1f°C %s\n",
				  _handyTime.LongString().c_str(),
				  _loopPersSecondCount,
				  _gpsParser.GetGpsBytesRec(),
				  (int)(100.0 * free / total),
				  temperature,
				  WiFi.localIP().toString().c_str());

	// Disable Access point mode
	if (WiFi.getMode() != WIFI_STA && WiFi.status() == WL_CONNECTED)
	{
		Logln("W105 - WiFi mode is not WIFI_STA, resetting");
		WiFi.softAPdisconnect(false);
		//_wifiManager.setConfigPortalTimeout(60);
		Logln("Set WIFI_STA mode");
		WiFi.mode(WIFI_STA);
		_swipePageIO.RefreshData();
		_lvCore.UpdateWiFiIndicator();
	}

	// Update display battery
	_lvCore.SetBatteryPercent(_powerManagementSystem.GetBatteryPercent(), _powerManagementSystem.IsCharging());

	// Performance text
	std::string perfText = StringPrintf("%d%% %.0fC %ddBm",
										(int)(100.0 * free / total),
										temperature,
										WiFi.RSSI());
	_display.SetPerformance(perfText);
	_slowLoopWaitTime = t;
}

//////////////////////////////////////////////////////////////////////////////
// Read the base location from the disk
void SaveBaseLocation(std::string newBaseLocation)
{
	_baseLocation = newBaseLocation;
	_myFiles.WriteFile(BASE_LOCATION_FILENAME, newBaseLocation.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// Check Wifi and reconnect
bool IsWifiConnected()
{
	// Is the WIFI connected?
	wl_status_t status = WiFi.status();
	if (_lastWifiStatus != status)
	{
		_lastWifiStatus = status;
		Logf("Wifi Status %d %s", status, WifiStatus(status));
		//	_display.SetWebStatus(status);
		_display.RefreshWiFiState();
		//_wifiFullResetTime = millis();

		// Reconnected
		if (status == WL_CONNECTED)
		{
			Logf("IP:%s Host:%s", WiFi.localIP().toString().c_str(), _mdnsHostName.c_str());
			// Setup the access point to prevend device getting stuck on a nearby network
			// auto res = _wifiManager.startConfigPortal(WiFi.getHostname(), AP_PASSWORD);
			// if (!res)
			//	Logln("Failed to start config Portal (Maybe cos non-blocked)");
			// else
			//	Logln("Config portal started");

			// Start the web portal
			//_wifiManager.startWebPortal();

			// TODO : Set the mDNS host name
			// TODO : Get latest time from the RTC
			// TODO : Save the SSID name
			_webPortal.OnConnected();
		}

		_swipePageIO.RefreshData();
		_lvCore.UpdateWiFiIndicator();
	}

	if (status == WL_CONNECTED)
		return true;

	if (status != WL_DISCONNECTED && _webPortal.GetConnectCount() > 0)
		return false;

	// Block here until we are connected again
	//	_webPortal.Setup();
	return false;

	// // Reset the WIFI if Disconnected (This seems to be unrecoverable)
	// auto delay = millis() - _wifiFullResetTime;
	// auto message = StringPrintf("[X:%d] FORCE RECONNECT", delay / 1000);
	// Serial.println(message.c_str());
	// _display.SetCell(message, DISPLAY_PAGE, DISPLAY_ROW);

	// // Start the connection process
	// // Logln("E310 - No WIFI");
	// unsigned long t = millis();
	// if (delay < WIFI_STARTUP_TIMEOUT)
	// 	return false;

	// //_wifiManager.resetSettings();

	// Logln("E107 - Try resetting WIfi");
	// _wifiFullResetTime = t;

	// // We will not block here until the WIFI is connected
	// _wifiManager.setConfigPortalBlocking(false);
	// _wifiManager.startConfigPortal(WiFi.getHostname(), AP_PASSWORD);
	// // WiFi.mode(WIFI_STA);
	// // wl_status_t beginState = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	// // Logf("WiFi Connecting %d %s\r\n", beginState, WifiStatus(beginState));
	// _display.RefreshWiFiState();

	return false;
}