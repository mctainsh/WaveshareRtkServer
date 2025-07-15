#pragma once

#include <SPI.h> // Needed for time
#include <SensorPCF85063.hpp>

#include <WiFi.h>
#include <time.h>
#include <lvgl.h>	  // Add this include for lv_label_set_text
#include <esp_sntp.h> // Include SNTP for time synchronization
#include "Global.h"
#include "HandyLog.h"
#include "HandyString.h"

#define TIME_SYNC_SHORT 60 * 1000		   // 2 minutes
#define TIME_SYNC_LONG 12 * 60 * 60 * 1000 // 12 hours

class HandyTime;
extern HandyTime _handyTime;

///////////////////////////////////////////////////////////////////////////////
// Time functions
// WARNING : This class is called by logger so do not log yourself
class HandyTime
{
private:
	long _gmtOffset_sec = 0;
	int _daylightOffset_sec = 0;

	bool _timeSyncEnabled = false;				   // Indicates we have WiFi or read a time from onboard clock
	unsigned long _lastSyncTime = 0;			   // Last time we synced the time
	unsigned long _syncInterval = TIME_SYNC_SHORT; // Default sync interval of 1 hour
	SensorPCF85063 _rtc;						   // Create an instance of the PCF85063 RTC
	bool _rtcWorking = false;					   // Time setup failed, so we will not try to read the time
	bool _rtcInitialized = false;				   // RTC has been initialized

public:
	void WiFiReady() { _timeSyncEnabled = true; }
	bool IsTimeSet() const { return _rtcInitialized; } // Check if the RTC has been initialized

	////////////////////////////////////////////////////////////
	// Check if we have a time value and use it
	void Setup()
	{
		//_instance = &this; // Set the instance to this class
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
		{
			Logln("[ERROR]:Clock integrity is not guaranteed; oscillator has stopped or has been interrupted");
			return;
		}

		// Read the current time from the RTC
		Logln("Clock integrity is guaranteed; oscillator has not been interrupted");
		RTC_DateTime datetime = _rtc.getDateTime();
		Logf("Read %d-%02d-%02d %02d:%02d", datetime.getYear(), datetime.getMonth(), datetime.getDay(), datetime.getHour(), datetime.getMinute());

		// If the RTC is not set, we will set it to a default value
		if (datetime.getYear() < 2020)
		{
			Logln("[ERROR]: RTC is not set, setting to default time");
			return;
		}

		struct tm info = datetime.toUnixTime();
		time_t epochTime = mktime(&info); // Convert to time_t
		struct timeval tv;
		tv.tv_sec = epochTime; // seconds since Jan 1, 1970
		tv.tv_usec = 0;		   // microseconds
		settimeofday(&tv, nullptr);

		// Check we set the time OK
		if (!getLocalTime(&info))
		{
			Logln("[ERROR]: Failed to set local time");
			return;
		}

		// Time successfully set from RTC
		_timeSyncEnabled = true;
		_lastSyncTime = millis(); // Set the last sync time to now
		_rtcInitialized = true;	  // RTC has been initialized successfully
		Logf("RTC Time set to %s", LongString().c_str());
	}

	///////////////////////////////////////////////////////////////////////////
	// Invalidate RTC to it doesn't get used after reboot
	void ResetRtc()
	{
		if (!_rtcWorking)
			return;
		_rtc.setDateTime(0, 1, 1, 0, 0, 0);
	}

	///////////////////////////////////////////////////////////////////////////
	// Enable time synchronization with NTP server
	// tzMinutes is the timezone offset in minutes, e.g., "60" for GMT
	void LoadTimezoneOffset(std::string tzMinutes)
	{
		if (tzMinutes.empty())
			return;
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

	////////////////////////////////////////////////////////////////////////////
	// Read the current time and issue a resync if necessary
	bool ReadTime(struct tm *info)
	{
		if (!_timeSyncEnabled)
			return false;

		// Check if a sync is needed
		if (millis() - _lastSyncTime > _syncInterval)
		{
			_lastSyncTime = millis();
			_syncInterval = TIME_SYNC_SHORT; // Set sync interval to 1 minute

			// Register the SNTP sync callback
			sntp_set_time_sync_notification_cb(HandyTime::OnTimeSyncCallback);

			// Request a new time sync
			auto msg = StringPrintf("Sync NTP time (%d)", _gmtOffset_sec);
			Serial.println(msg.c_str());
			Logln(msg.c_str(), false);
			configTime(_gmtOffset_sec, _daylightOffset_sec, "pool.ntp.org", "time.nist.gov", "time.google.com");
		}

		// Try to get the local time
		if (getLocalTime(info))
		{
			// Do we have the first good time?
			if (!_rtcInitialized && info->tm_year > 120)
				_rtcInitialized = true; // Set RTC initialized flag if we have a valid time
			return true;
		}

		// If we failed to get the local time, try again after a short delay
		// .. program runs real slow here
		_syncInterval = TIME_SYNC_SHORT; // Set sync interval to 1 minute
		return false;					 // Failed to get local time
	}

	//////////////////////////////////////////////////////////////////
	// Callback function for time synchronization
	// .. This is because the config time function does not block
	// .. and we need to wait for the time to be set
	static void OnTimeSyncCallback(struct timeval *tv)
	{
		Logln(StringPrintf("Time synchronized via NTP %u", tv->tv_sec).c_str(), false);
		if (tv == nullptr || tv->tv_sec == 0)
		{
			Logln("*** ERROR : HandyTime OnTimeSyncCallback failed", false);
			return;
		}
		tm info;
		if (!_handyTime._rtcWorking || !getLocalTime(&info))
			return;
		Logln("NTP time synced", false);

		_handyTime._rtc.setDateTime(info.tm_year + 1900, info.tm_mon + 1, info.tm_mday,
									info.tm_hour, info.tm_min, info.tm_sec);
		_handyTime._syncInterval = TIME_SYNC_LONG;
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
	std::string Format(const char *format)
	{
		struct tm timeinfo;
		if (!ReadTime(&timeinfo))
			return Uptime(millis());

		const int len = 64; // Adjust size as needed
		char time[len];
		if (strftime(time, len, format, &timeinfo) == 0)
			return std::string(format);
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
