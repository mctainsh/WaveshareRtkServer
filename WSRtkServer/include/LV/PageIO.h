#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"
#include "Hardware/SdFile.h"
#include <WiFi.h>
#include <SPIFFS.h>
#include "Web/WebPortal.hpp"

extern SDFile _sdFile;
extern std::string _mdnsHostName;
extern WebPortal _webPortal;

class PageIO;
PageIO *_pageIO = nullptr;

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class PageIO : public SwipePageBase
{
public:
	void Show()
	{
		lv_obj_t *scr = lv_obj_create(NULL);
		CreatePanel(scr, LV_SYMBOL_DIRECTORY " Disk & Memory", 0xA4CCD9);

		// Table
		CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels

		// SD Card
		AppendRowTitle(LV_SYMBOL_SD_CARD " SD Card", TblFormat::Highlight);
		AppendRowTitle("State");
		AppendRowTitle("Space");

		// Flash memory
		AppendRowTitle(LV_SYMBOL_DRIVE " Flash", TblFormat::Highlight);
		AppendRowTitle("Sketch", TblFormat::Right);
		AppendRowTitle("Size", TblFormat::Right);
		AppendRowTitle("SPIFFS", TblFormat::Right);

		// PSRAM
		AppendRowTitle("PSRAM", TblFormat::Right);

		// WiFi
		AppendRowTitle(LV_SYMBOL_WIFI " WiFi", TblFormat::Highlight);
		AppendRowTitle("Status");
		AppendRowTitle("Strength");
		AppendRowTitle("A/P Name");
		AppendRowTitle("IP Address");
		AppendRowTitle("Host Name");
		AppendRowTitle("Type");
		AppendRowTitle("Reconnects");

		AddCloseButton(scr, PageIO::OnClose); // Add a close button to the bottom of the page

		// Animate the screen load
		lv_screen_load_anim(scr, lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OVER_LEFT, 300, 0, false);
		_pageIO = this;
	}

	///////////////////////////////////////////////////////////////////////////
	// Uplate the table with the current data
	void RefreshData()
	{
		// SD Card
		SetTableValue(1, _sdFile.GetState().c_str());
		SetTableValue(2, _sdFile.GetDriveSpace().c_str());

		// Flash details
		SetTableValue(4, MakeKbPercent(ESP.getSketchSize(), ESP.getFreeSketchSpace()).c_str());

		SetTableValue(5, (ToThousands(ESP.getFlashChipSize() / KB) + " kb").c_str());
		SetTableValue(6, MakeKbPercent(SPIFFS.usedBytes(), SPIFFS.totalBytes()).c_str());

		// PSRAM
		SetTableValue(7, MakeKbPercent(ESP.getPsramSize() - ESP.getFreePsram(), ESP.getPsramSize()).c_str());

		// WiFi
		auto status = WiFi.status();
		SetTableString(9, WifiStatus(status));
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
			SetTableValue(10, "Not Connected");
		else
			SetTableValue(10, (std::to_string(strength) + "dBm " + strengthTitle).c_str());
		SetTableValue(11, WiFi.getHostname());
		if (status == WL_CONNECTED)
		{
			SetTableValue(12, WiFi.localIP().toString().c_str());
			SetTableValue(13, (_mdnsHostName + ".local").c_str());
		}
		else
		{
			SetTableValue(12, "X -> 192.168.4.1");
			SetTableValue(13, "");
		}
		SetTableValue(14, WiFiModeText(WiFi.getMode()));
		SetTableValue(15, std::to_string(_webPortal.GetConnectCount() - 1).c_str());
	}

	
	/////////////////////////////////////////////////////////////////////////////////
	// Event handler for the button
	static void OnClose(lv_event_t *e)
	{
		if (lv_event_get_code(e) != LV_EVENT_CLICKED)
			return;
		if (_pageIO != nullptr)
			delete _pageIO;
		_pageIO = nullptr; // Clear the pointer to the PagePower instance
		lv_screen_load_anim(_lvCore.GetHomeScreen(), lv_screen_load_anim_t::LV_SCR_LOAD_ANIM_OUT_RIGHT, 300, 0, false);
	}
};
