#pragma once

#include <SPI.h> // Needed for time
#include <SensorPCF85063.hpp>

#include <WiFi.h>
#include "time.h"
#include "HandyLog.h"
#include "Global.h"
#include <lvgl.h> // Add this include for lv_label_set_text

///////////////////////////////////////////////////////////////////////////////
// Time functions
// WARNING : This class is called by logger so do not log yourself
class HandyTime
{
private:
	const char *_ntpServer = "pool.ntp.org";
	long _gmtOffset_sec = 0;
	int _daylightOffset_sec = 0;

	bool _timeSyncEnabled = false;	 // Indicate if time sync is enabled
	unsigned long _lastSyncTime = 0; // Last time we synced the time
	unsigned long _syncInterval = 0; // Default sync interval of 1 hour

	SensorPCF85063 _rtc;	  // Create an instance of the PCF85063 RTC
	bool _rtcWorking = false; // Time setup failed, so we will not try to read the time
public:
	void Setup()
	{
		// Initialize the RTC module using I2C with specified SDA and SCL pins
		if (!_rtc.begin(Wire, I2C_SDA, I2C_SCL))
		{
			Logln("Failed to find PCF85063 'Clock' - check your wiring!");
			_rtcWorking = false; // RTC initialization failed
			return;				 // Do not continue if RTC initialization fails
		}
		Logln("PCF85063 RTC initialized successfully");
		_rtcWorking = true; // RTC initialized successfully

		if (!_rtc.isClockIntegrityGuaranteed())
			Logln("[ERROR]:Clock integrity is not guaranteed; oscillator has stopped or has been interrupted");
	}

	void EnableTimeSync(std::string tzMinutes)
	{
		if (!tzMinutes.empty())
		{
			try
			{
				float minutes = std::stof(tzMinutes);
				_gmtOffset_sec = static_cast<long>(minutes * 60); // Convert minutes to seconds
				_daylightOffset_sec = 0;						  // Default to no daylight saving time
				Logf("Timezone set to %s minutes (%ld seconds)", tzMinutes.c_str(), _gmtOffset_sec);
			}
			catch (const std::invalid_argument &e)
			{
				Logf("Invalid timezone minutes: %s", tzMinutes.c_str());
			}
			catch (const std::out_of_range &e)
			{
				Logf("Timezone minutes out of range: %s", tzMinutes.c_str());
			}
		}
		_timeSyncEnabled = true;
	}

	////////////////////////////////////////////////////////////////////////////
	// Read the current time and issue a resync if necessary
	bool ReadTime(struct tm *info)
	{
		if (!_timeSyncEnabled)
			return false;

		// Check if a sync is needed
		if (millis() - _lastSyncTime > _syncInterval)
		{
			Logln("Sync NTP time", false);
			configTime(_gmtOffset_sec, _daylightOffset_sec, _ntpServer);
			_lastSyncTime = millis();

			// Try to get the local time
			if (getLocalTime(info))
			{
				Logln("NTP time synced", false);
				_syncInterval = 60 * 60 * 1000; // Set sync interval to 1 hour
				if (_rtcWorking)
					_rtc.setDateTime(info->tm_year + 1900, info->tm_mon + 1, info->tm_mday,
									 info->tm_hour, info->tm_min, info->tm_sec);
				return true;
			}
			else
			{
				Logln("ERROR : Failed to sync NTP time", false);
				_syncInterval = 60 * 1000; // Set sync interval to 1 minute
				return false;
			}
		}

		// Try to get the local time
		if (getLocalTime(info))
			return true;

		// If we failed to get the local time, try again after a short delay
		// .. program runs real slow here
		_syncInterval = 60 * 1000; // Set sync interval to 1 minute
		return false;			   // Failed to get local time
	}

	///////////////////////////////////////////////////////////////////////////
	// Update the page title with the current date and time
	void UpdatePageTitle(lv_obj_t *plabel)
	{
		if (!_rtcWorking || !plabel)
			return;

		struct tm timeinfo;
		if (!ReadTime(&timeinfo))
			return; // Failed to read time

		// Format the date and time
		char buf[64];
		size_t written = strftime(buf, sizeof(buf), "%A, %B %d %Y %H:%M:%S", &timeinfo);
		if (written < 1)
			return; // Failed to format time

		lv_label_set_text(plabel, buf);
	}

	///////////////////////////////////////////////////////////////////////////
	// Get a formatted time
	std::string LongString()
	{
		struct tm timeinfo;
		if (!ReadTime(&timeinfo))
			return Uptime(millis());

		// 2025-06-20 22:35:54
		const int len = 22;
		char time[len];
		if (strftime(time, len, "%Y-%02m-%02d %H:%M:%S", &timeinfo) == 0)
			return std::string("Error formatting time");
		return std::string(time);
	}

	///////////////////////////////////////////////////////////////////////////
	// Get a compressed format like 2025-06-22 435544
	std::string FileSafe()
	{
		struct tm timeinfo;
		if (!ReadTime(&timeinfo))
			return Uptime(millis());

		// 2025-06-20 22:35:54
		const int len = 22;
		char time[len];
		if (strftime(time, len, "%Y-%02m-%02d %H%M%S", &timeinfo) == 0)
			return std::string("Error formatting time");
		return std::string(time);
	}

	///////////////////////////////////////////////////////////////////////////
	// Get a short time string HH:MM:SS
	std::string HH_MM_SS()
	{
		struct tm timeinfo;
		if (!ReadTime(&timeinfo))
			return Uptime(millis());

		// 22:35:54
		const int len = 10;
		char time[len];
		if (strftime(time, len, "%H:%M:%S", &timeinfo) == 0)
			return std::string("Error formatting time");
		return std::string(time);
	}

	///////////////////////////////////////////////////////////////////////////
	// Get a short time string HH:MM:SS
	std::string ddd_HH_MM_SS()
	{
		struct tm timeinfo;
		if (!ReadTime(&timeinfo))
			return Uptime(millis());

		// Wed 22:35:54
		const int len = 15;
		char time[len];
		if (strftime(time, len, "%a %H:%M:%S", &timeinfo) == 0)
			return std::string("Error formatting time");
		return std::string(time);
	}
};
