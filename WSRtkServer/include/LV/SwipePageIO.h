#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"
#include "Hardware/SdFile.h"
#include <WiFi.h>
#include <SPIFFS.h>

extern SDFile _sdFile;
extern std::string _mdnsHostName;

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class SwipePageIO : public SwipePageBase
{
private:

public:
	void Create(lv_obj_t *parentGroupPanel)
	{
		CreatePanel(parentGroupPanel, "Disk & Memory", 0x0000FF);

		// Table
		CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels

		// SD Card
		AppendRowTitle("SD Card", TblFormat::Highlight);
		AppendRowTitle("State");
		AppendRowTitle("Space");

		// Flash memory
		AppendRowTitle("Flash", TblFormat::Highlight);
		AppendRowTitle("Sketch", TblFormat::Right);
		AppendRowTitle("Size", TblFormat::Right);
		AppendRowTitle("SPIFFS");

		// PSRAM
		AppendRowTitle("PSRAM");

		// WiFi
		AppendRowTitle("WiFi", TblFormat::Highlight);
		AppendRowTitle("Strength");
		AppendRowTitle("A/P Name");
		AppendRowTitle("IP Address");
		AppendRowTitle("Host Name");
	}

	void RefreshData()
	{
		// SD Card
		SetTableValue(1, _sdFile.GetState().c_str());
		SetTableValue(2, _sdFile.GetDriveSpace().c_str());

		// Flash details
		SetTableValue(4, MakeKbPercent( ESP.getSketchSize(), ESP.getFreeSketchSpace()).c_str());

		SetTableValue(5, (ToThousands(ESP.getFlashChipSize()/KB)+ " kb").c_str());
		SetTableValue(6, MakeKbPercent(SPIFFS.usedBytes(), SPIFFS.totalBytes()).c_str());

		// PSRAM
		SetTableValue(7, MakeKbPercent(ESP.getFreePsram(), ESP.getPsramSize()).c_str());

		// WiFi
		auto strength = WiFi.RSSI();
		std::string strengthTitle = "Unusable";
		if (strength > -30)
			strengthTitle = "Excellent";
		else if (strength > -67)
			strengthTitle = "Very Good";
		else if (strength > -70)
			strengthTitle = "Okay";
		else if (strength > -80)
			strengthTitle = "Not Good";
		if (strength == 0)
			SetTableValue(9, "Not Connected");
		else
			SetTableValue(9, (std::to_string(strength) + "dBm " + strengthTitle).c_str());
		SetTableValue(10, WiFi.getHostname());
		SetTableValue(11, WiFi.localIP().toString().c_str());
		SetTableValue(12, (_mdnsHostName + ".local").c_str());
		SetTableValue(13,
			WiFi.getMode() == WIFI_MODE_NULL
				? "N/A"
				: (WiFi.getMode() == WIFI_STA
					   ? "Station"
					   : (WiFi.getMode() == WIFI_AP
							  ? "Access Point"
							  : (WiFi.getMode() == WIFI_AP_STA ? "Access Point + Station"
															   : "Unknown"))));
	}
};
