#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"
#include "Web/WebPortal.hpp"
#include "GpsParser.h"

extern WebPortal _webPortal; // Web portal instance
extern GpsParser _gpsParser; // GPS parser instance

///////////////////////////////////////////////////////////////////////////////
// This is a page with a number of controls on it is added to a scrolling panel
class SwipePageHome : public SwipePageBase
{
private:
	// PanelLabelValue _panelLabelValue1; // Create an instance of PanelLabelValue
	// PanelLabelValue _panelLabelValue2; // Create an instance of PanelLabelValue
	// PanelLabelValue _panelLabelValue3; // Create an instance of PanelLabelValue
	int _hostOrApNameRow = 0;

public:
	void Create(lv_obj_t *parentGroupPanel)
	{
		CreatePanel(parentGroupPanel, LV_SYMBOL_USB " GPS Status", 0xA2E1DB); // Create the panel for this page
		//_panelLabelValue1.Create(_uiPanelPage, "TTime", "00:00:00");			 // Create a panel with label and value
		//_panelLabelValue2.Create(_uiPanelPage, "Other", "00:00:00");			 // Create a panel with label and value
		//_panelLabelValue3.Create(_uiPanelPage, "More message Time", "00:00:00"); // Create a panel with label and value

		CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels
		AppendRowTitle("GPS", TblFormat::Highlight);
		//AppendRowTitle("Bytes", TblFormat::Right);
		AppendRowTitle("Resets", TblFormat::Right);
		AppendRowTitle("Reinitialize", TblFormat::Right);
		AppendRowTitle("ASCII Pkts", TblFormat::Right);
		AppendRowTitle("RTK Pkts", TblFormat::Right);

		// WiFi
		AppendRowTitle(LV_SYMBOL_WIFI " WiFi", TblFormat::Highlight);
		AppendRowTitle("Status");
		AppendRowTitle("Strength");
		AppendRowTitle("IP Address");
		_hostOrApNameRow = AppendRowTitle("AP OR HOST");
		//AppendRowTitle("Type");
		AppendRowTitle("Reconnects");
	}

	void RefreshData()
	{
		// GPS Data
		// SetTableValue( 1, _gpsParser.GetGpsBytesRec());
		SetTableValue(1, _gpsParser.GetGpsResetCount());
		SetTableValue(2, _gpsParser.GetGpsReinitialize());
		SetTableValue(3, _gpsParser.GetAsciiMsgCount());
		SetTableValue(4, _gpsParser.GetRtkMsgCount());

		// WiFi
		const int x = 5;
		auto status = WiFi.status();
		SetTableString(x + 1, WifiStatus(status));
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
			SetTableValue(x + 2, "Not Connected");
		else
			SetTableValue(x + 2, (std::to_string(strength) + "dBm " + strengthTitle).c_str());

		

//OR
		//AppendRowTitle("Host Name");
		//


		if (status == WL_CONNECTED)
		{
			SetTableValue(x + 3, WiFi.localIP().toString().c_str());
			lv_table_set_cell_value(_table, x + 4, 0, "Host name");
			SetTableValue(x + 4, (_mdnsHostName + ".local").c_str());
		}
		else
		{
			SetTableValue(x + 3, "X -> 192.168.4.1");
			lv_table_set_cell_value(_table, x + 4, 0, "A/P Name");
			SetTableValue(x + 4, WiFi.getHostname());
		}
		SetTableValue(x + 5, std::to_string(_webPortal.GetConnectCount() - 1).c_str());
	}
};
