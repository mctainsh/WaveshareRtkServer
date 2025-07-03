#include <Arduino.h>
#include "Global.h"
#include "LV/LVCore.h"
#include "UI/screens/ui_MainScreen.h"
#include <LV/SwipePageGps.h>
#include <LV/SwipePageIO.h>
#include <SDFile.h>
#include <LV/SwipePageSettings.h>

#include <SPI.h> // Needed for time
#include <SensorPCF85063.hpp>


SensorPCF85063 rtc;	 // Create an instance of the PCF85063 RTC 

SDFile _sdFile; // Create an instance of SDFile
LVCore _lvCore;

// Pages
SwipePageGps _systemPageGps;
SwipePageGps _swipePageGps;
SwipePageIO _swipePageIO;
SwipePageSettings _swipePageSettings;

std::string _mdnsHostName;

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

	//// Initialize the QMI8658 IMU sensor
	// Try to initialize the RTC module using I2C with specified SDA and SCL pins


	if (!rtc.begin(Wire, I2C_SDA, I2C_SCL))
	{
		Serial.println("Failed to find PCF85063 'Clock' - check your wiring!");
		// Enter an infinite loop to halt the program
		while (1)
		{
			Serial.println("Failed to find PCF85063 - check your wiring!");
		}
	}


	// Set the defined date and time on the RTC
	//   rtc.setDateTime(year, month, day, hour, minute, second);

	if (!rtc.isClockIntegrityGuaranteed())
	{
		Serial.println("[ERROR]:Clock integrity is not guaranteed; oscillator has stopped or has been interrupted");
	}

	_lvCore.Setup(); // Initialize LVGL and the display
	Serial.println("Setup done");

	// Create the main screen (Use lv_scr_act())
	ui_MainScreen_screen_init();

	// Create the GPS status page
	_systemPageGps.Create(UIPageGroupPanel);	 // Create the GPS status page and add
	_swipePageIO.Create(UIPageGroupPanel);		 // Create the GPS status page and add
	_swipePageSettings.Create(UIPageGroupPanel); // Create the settings page and add

	// Fix the startup scroll offset error
	lv_obj_scroll_to_view(_systemPageGps.GetPanel(), LV_ANIM_OFF);

	_swipePageIO.RefreshData();
}



///////////////////////////////////////////////////////////////////////////////
// This function is called repeatedly in a loop
// It handles the LVGL tasks and allows the GUI to update
void loop()
{
	lv_task_handler(); // Let the GUI do its work
					   //	_panelLabelValue1.SetText(("Time: " + std::to_string(millis())).c_str());
					   //	_panelLabelValue2.SetText(("Time: " + std::to_string(millis())).c_str());
					   //	_panelLabelValue3.SetText(("Time: " + std::to_string(millis())).c_str());


	  return;

		char buf[64];

		struct tm timeinfo;
		// Get the time C library structure
		rtc.getDateTime(&timeinfo);

		// Format the output using the strftime function
		// For more formats, please refer to :
		// https://man7.org/linux/man-pages/man3/strftime.3.html

		size_t written = strftime(buf, 64, "%A, %B %d %Y %H:%M:%S", &timeinfo);

		if (written != 0)
		{
			Serial.println(buf);
		}

		written = strftime(buf, 64, "%b %d %Y %H:%M:%S", &timeinfo);
		if (written != 0)
		{
			Serial.println(buf);
		}

		written = strftime(buf, 64, "%A, %d. %B %Y %I:%M%p", &timeinfo);
		if (written != 0)
		{
			Serial.println(buf);
		}
}
