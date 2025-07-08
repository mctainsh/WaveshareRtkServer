#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"
#include "Web/WebPortal.hpp"

extern WebPortal _webPortal;

///////////////////////////////////////////////////////////////////////////////
// This is a page with a number of controls on it is added to a scrolling panel
class SwipePageGps : public SwipePageBase
{
private:
	PanelLabelValue _panelLabelValue1; // Create an instance of PanelLabelValue
	PanelLabelValue _panelLabelValue2; // Create an instance of PanelLabelValue
	PanelLabelValue _panelLabelValue3; // Create an instance of PanelLabelValue

public:
	void Create(lv_obj_t *parentGroupPanel)
	{
		CreatePanel(parentGroupPanel, LV_SYMBOL_GPS " GPS Status", 0xFF00FF);	 // Create the panel for this page
		_panelLabelValue1.Create(_uiPanelPage, "TTime", "00:00:00");			 // Create a panel with label and value
		_panelLabelValue2.Create(_uiPanelPage, "Other", "00:00:00");			 // Create a panel with label and value
		_panelLabelValue3.Create(_uiPanelPage, "More message Time", "00:00:00"); // Create a panel with label and value

		CreateTable(_uiPanelPage, lv_pct(100)); // Create a table with a height of 200 pixels
		AppendRowTitle("Name", TblFormat::Highlight);
		AppendRowTitle("Apple");
		// WiFi
		AppendRowTitle(LV_SYMBOL_WIFI " WiFi", TblFormat::Highlight);
		AppendRowTitle("Status");
		AppendRowTitle("Strength");
		AppendRowTitle("A/P Name");
		AppendRowTitle("IP Address");
		AppendRowTitle("Host Name");
		AppendRowTitle("Type");
		AppendRowTitle("Reconnects");
	}

	void RefreshData()
	{
		// WiFi
		auto status = WiFi.status();
		SetTableString(1, WifiStatus(status));
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
			SetTableValue(2, "Not Connected");
		else
			SetTableValue(2, (std::to_string(strength) + "dBm " + strengthTitle).c_str());

		SetTableValue(3, WiFi.getHostname());
		if (status == WL_CONNECTED)
		{
			SetTableValue(4, WiFi.localIP().toString().c_str());
			SetTableValue(5, (_mdnsHostName + ".local").c_str());
		}
		else
		{
			SetTableValue(4, "X -> 192.168.4.1");
			SetTableValue(5, "");
		}
		SetTableValue(6, WiFiModeText(WiFi.getMode()));
		SetTableValue(7, std::to_string(_webPortal.GetConnectCount() - 1).c_str());
	}
};
