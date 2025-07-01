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

		// SD Card
		_panelSDState.Create(_uiPanelPage, "SD Card", "");
		_panelSpace.Create(_uiPanelPage, "Space", "");

		// Flash memory
		_panelSketchSize.Create(_uiPanelPage, "Sketch Size", "");
		_panelFreeSketchSpace.Create(_uiPanelPage, "Free Sketch Space", "");
		_panelFlashChipSize.Create(_uiPanelPage, "Flash Chip Size", "");
		_panelSPIFFS.Create(_uiPanelPage, "SPIFFS", "");

		// PSRAM
		_panelPSRAM.Create(_uiPanelPage, "PSRAM", "");

		// WiFi
		_panelWiFiStrength.Create(_uiPanelPage, "WiFi Strength", "");
		_panelAP_Name.Create(_uiPanelPage, "A/P Name", "");
		_panelIPAddress.Create(_uiPanelPage, "IP Address", "");
		_panelHostName.Create(_uiPanelPage, "Host Name", "");
		_panelWiFiMode.Create(_uiPanelPage, "WiFi Mode", "");
	}

	void RefreshData()
	{
		const int KB = 1024;

		// SD Card
		_panelSDState.SetText(_sdFile.GetState().c_str());
		_panelSpace.SetText(_sdFile.GetDriveSpace().c_str());

		// Flash details
		_panelSketchSize.SetText(std::to_string(ESP.getSketchSize()).c_str());
		_panelFreeSketchSpace.SetText(std::to_string(ESP.getFreeSketchSpace()).c_str());
		_panelFlashChipSize.SetText(std::to_string(ESP.getFlashChipSize()).c_str());
		size_t totalBytes = SPIFFS.totalBytes();
		size_t usedBytes = SPIFFS.usedBytes();
		_panelSPIFFS.SetText(totalBytes > 0 ? (std::to_string(usedBytes / KB) + " / " + std::to_string(totalBytes / KB) + "kb  " + std::to_string((int)(100.0 * usedBytes / totalBytes)) + "%").c_str() : "Not mounted");

		// PSRAM
		usedBytes = ESP.getFreePsram();
		totalBytes = ESP.getPsramSize();
		_panelPSRAM.SetText(totalBytes > 0 ? (std::to_string(usedBytes / KB) + " / " + std::to_string(totalBytes / KB) + "kb  " + std::to_string((int)(100.0 * usedBytes / totalBytes)) + "%").c_str() : "Not mounted");

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
			_panelWiFiStrength.SetText("Not Connected");
		else
			_panelWiFiStrength.SetText((std::to_string(strength) + "dBm " + strengthTitle).c_str());

		_panelAP_Name.SetText(WiFi.getHostname());
		_panelIPAddress.SetText(WiFi.localIP().toString().c_str());
		_panelHostName.SetText((_mdnsHostName + ".local").c_str());
		_panelWiFiMode.SetText(
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
