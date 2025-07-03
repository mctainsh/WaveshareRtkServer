#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"
#include <SDFile.h>
#include <WiFi.h>
#include <SPIFFS.h>

extern SDFile _sdFile;
extern std::string _mdnsHostName;

///////////////////////////////////////////////////////////////////////////////
// This is a page with information about drives and memory
class SwipePageIO : public SwipePageBase
{
private:
	// SD Card
	PanelLabelValue _panelSDState;
	PanelLabelValue _panelSpace;

	// Flash memory
	PanelLabelValue _panelSketchSize;
	PanelLabelValue _panelFreeSketchSpace;
	PanelLabelValue _panelFlashChipSize;
	PanelLabelValue _panelSPIFFS;

	// PSRAM
	PanelLabelValue _panelPSRAM;

	// WiFi
	PanelLabelValue _panelWiFiStrength;
	PanelLabelValue _panelAP_Name;
	PanelLabelValue _panelIPAddress;
	PanelLabelValue _panelHostName;
	PanelLabelValue _panelWiFiMode;

public:
	void Create(lv_obj_t *parentGroupPanel)
	{
		CreatePanel(parentGroupPanel, "Disk & Memory", 0x0000FF);

	
		// // SD Card
		// _panelSDState.Create(_uiPanelPage, "SD Card", "");
		// _panelSpace.Create(_uiPanelPage, "Space", "");

		// // Flash memory
		// _panelSketchSize.Create(_uiPanelPage, "Sketch Size", "");
		// _panelFreeSketchSpace.Create(_uiPanelPage, "Free Sketch Space", "");
		// _panelFlashChipSize.Create(_uiPanelPage, "Flash Chip Size", "");
		// _panelSPIFFS.Create(_uiPanelPage, "SPIFFS", "");

		// // PSRAM
		// _panelPSRAM.Create(_uiPanelPage, "PSRAM", "");

		// // WiFi
		// _panelWiFiStrength.Create(_uiPanelPage, "WiFi Strength", "");
		// _panelAP_Name.Create(_uiPanelPage, "A/P Name", "");
		// _panelIPAddress.Create(_uiPanelPage, "IP Address", "");
		// _panelHostName.Create(_uiPanelPage, "Host Name", "");
		// _panelWiFiMode.Create(_uiPanelPage, "WiFi Mode", "");


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
