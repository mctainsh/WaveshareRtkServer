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
extern PageIO *_pageIO; // Pointer to the PageIO instance

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class PageIO : public SwipePageBase
{
private:
public:
	PageIO()
	{
		_title = LV_SYMBOL_DIRECTORY " Disk & Memory";
		_screen = lv_obj_create(NULL);
		CreatePanel(_screen, _title, 0xA4CCD9);

		// Table
		CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels

		// Firmware version
		AppendRowTitle("Firmware");
		SetTableValue(0, "Version : " APP_VERSION);

		// SD Card
		AppendRowTitle(LV_SYMBOL_SD_CARD " SD Card", TblFormat::Highlight);
		AppendRowTitle("State");
		AppendRowTitle("Space");

		// Flash memory
		AppendRowTitle(LV_SYMBOL_SAVE " Flash", TblFormat::Highlight);
		AppendRowTitle("Sketch", TblFormat::Right);
		AppendRowTitle("Size", TblFormat::Right);
		AppendRowTitle("SPIFFS", TblFormat::Right);

		// PSRAM
		AppendRowTitle("PSRAM", TblFormat::Right);

		// WiFi
		AppendRowTitle(LV_SYMBOL_WIFI " WiFi", TblFormat::Highlight);
		AppendRowTitle("Status");
		AppendRowTitle("Strength");
		AppendRowTitle("SSID");
		AppendRowTitle("IP Address");
		AppendRowTitle("[Starting]");
		AppendRowTitle("Type");
		AppendRowTitle("Reconnects");

		AddCloseButton(_screen, SwipePageBase::OnClose, this); // Add a close button to the bottom of the page
	}

	///////////////////////////////////////////////////////////////////////////
	// Uplate the table with the current data
	void RefreshData()
	{
		if (!Ready())
			return;

		// SD Card
		SetTableValue(2, _sdFile.GetState().c_str());
		SetTableValue(3, _sdFile.GetDriveSpace().c_str());

		// Flash details
		SetTableValue(5, MakeKbPercent(ESP.getSketchSize(), ESP.getFreeSketchSpace()).c_str());

		SetTableValue(6, (ToThousands(ESP.getFlashChipSize() / KB) + " kb").c_str());
		SetTableValue(7, MakeKbPercent(SPIFFS.usedBytes(), SPIFFS.totalBytes()).c_str());

		// PSRAM
		SetTableValue(8, MakeKbPercent(ESP.getPsramSize() - ESP.getFreePsram(), ESP.getPsramSize()).c_str());

		// WiFi
		int n = 10;
		auto status = WiFi.status();
		SetTableString(n + 0, WifiStatus(status));
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
			SetTableValue(n + 1, "Not Connected");
		else
			SetTableValue(n + 1, (std::to_string(strength) + "dBm " + strengthTitle).c_str());
		SetTableValue(n + 2, WiFi.SSID().c_str());

		const char *name;
		if (status == WL_CONNECTED)
		{
			SetTableValue(n + 3, WiFi.localIP().toString().c_str());
			SetTableValue(n + 4, (_mdnsHostName + ".local").c_str());
			name = "Host Name";
		}
		else
		{
			SetTableValue(n + 3, "X -> 192.168.4.1");
			SetTableValue(n + 4, WiFi.getHostname());
			name = "A/P Name";
		}
		lv_table_set_cell_value(_table, n + 4, 0, name);


		// // IP Address
		// if (status == WL_CONNECTED)
		// 	SetTableValue(x + 3, WiFi.localIP().toString().c_str());
		// else
		// 	SetTableValue(x + 3, "X -> 192.168.4.1");

		// // Host name or A/P name
		// const char *name;
		// if (WiFi.getMode() == WIFI_STA)
		// {
		// 	SetTableValue(x + 4, (_mdnsHostName + ".local").c_str());
		// 	name = "Host Name";
		// }
		// else
		// {
		// 	SetTableValue(x + 4, WiFi.getHostname());
		// 	name = "A/P Name";
		// }
		// lv_table_set_cell_value(_table, x + 4, 0, name);




		SetTableValue(n + 5, WiFiModeText(WiFi.getMode()));
		SetTableValue(n + 6, std::to_string(_webPortal.GetConnectCount() - 1).c_str());
	}
};
