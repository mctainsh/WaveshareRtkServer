#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"
#include "Web/WebPortal.hpp"
#include "GpsParser.h"

extern WebPortal _webPortal;	  // Web portal instance
extern GpsParser _gpsParser;	  // GPS parser instance
extern NTRIPServer _ntripServer0; // NTRIP server  instance
extern NTRIPServer _ntripServer1; // ..
extern NTRIPServer _ntripServer2; // ..

///////////////////////////////////////////////////////////////////////////////
// This is a page with a number of controls on it is added to a scrolling panel
class SwipePageHome : public SwipePageBase
{
private:
	// PanelLabelValue _panelLabelValue1; // Create an instance of PanelLabelValue
	// PanelLabelValue _panelLabelValue2; // Create an instance of PanelLabelValue
	// PanelLabelValue _panelLabelValue3; // Create an instance of PanelLabelValue
	lv_obj_t *_ntripTable = nullptr; // NTRIP table

public:
	void Create(lv_obj_t *parentGroupPanel)
	{
		CreatePanel(parentGroupPanel, LV_SYMBOL_HOME " System Status", 0xA2E1DB); // Create the panel for this page

		CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels
		AppendRowTitle("Up time", TblFormat::Right);
		AppendRowTitle("GPS", TblFormat::Highlight | TblFormat::Right);
		AppendRowTitle("Resets", TblFormat::Right);
		AppendRowTitle("Reinitialize", TblFormat::Right);
		AppendRowTitle("ASCII Pkts", TblFormat::Right);
		AppendRowTitle("RTK Pkts", TblFormat::Right);

		// WiFi
		AppendRowTitle(LV_SYMBOL_WIFI " WiFi", TblFormat::Highlight);
		// AppendRowTitle("Status");
		AppendRowTitle("Strength");
		AppendRowTitle("IP Address");
		AppendRowTitle("AP OR HOST");
		AppendRowTitle("Reconnects");

		// NTRIP Servers
		lv_obj_t *backup = _table;					// Save the current table
		_rowCount = 0;								// Reset the row count
		CreateTable(_uiPanelPage, LV_SIZE_CONTENT); // Create a table with a height of 200 pixels
		lv_table_set_col_width(_table, 0, 78);
		lv_table_set_col_width(_table, 1, 78);
		lv_table_set_col_width(_table, 2, 78);
		lv_table_set_col_width(_table, 3, 78);

		AppendRowTitle("Server", TblFormat::Highlight | TblFormat::SingleLine | TblFormat::Bold, 3);
		AppendRowTitle("Status", TblFormat::SingleLine, 3);
		AppendRowTitle("Reconne.", TblFormat::Right, 3);
		AppendRowTitle("Packets", TblFormat::Right, 3);
		AppendRowTitle("Timeouts", TblFormat::Right, 3);

		SetTableString(0, ShortServerName(_ntripServer0.GetAddress()), 1);
		SetTableString(0, ShortServerName(_ntripServer1.GetAddress()), 2);
		SetTableString(0, ShortServerName(_ntripServer2.GetAddress()), 3);

		// Restore the table
		_ntripTable = _table; // Save the NTRIP table
		_table = backup;
	}

	///////////////////////////////////////////////////////////////////////////
	// This function is called when the page is shown and every second to refresh it
	void RefreshData(unsigned long wifiAgeMs)
	{
		SetTableString(0, UptimeDMS(millis())); // Set the up time
		// GPS Data
		SetTableValue(1, _gpsParser.GetGpsBytesRec());
		SetTableValue(2, _gpsParser.GetGpsResetCount());
		SetTableValue(3, _gpsParser.GetGpsReinitialize());
		SetTableValue(4, _gpsParser.GetAsciiMsgCount());
		SetTableValue(5, _gpsParser.GetRtkMsgCount());

		// WiFi
		const int x = 5;
		auto status = WiFi.status();

		// Are we trying to reconnect?
		std::string statusText = WifiStatus(status);
		if (wifiAgeMs > 2500)
			statusText = statusText + "   [T-" + std::to_string((WIFI_RESTART_TIMEOUT - wifiAgeMs) / 1000) + "]";
		SetTableString(x + 1, statusText);
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

		// IP Address
		if (status == WL_CONNECTED)
			SetTableValue(x + 3, WiFi.localIP().toString().c_str());
		else
			SetTableValue(x + 3, "X -> 192.168.4.1");

		// Host name or A/P name
		const char *name;
		if (WiFi.getMode() == WIFI_STA)
		{
			SetTableValue(x + 4, (_mdnsHostName + ".local").c_str());
			name = "Host Name";
		}
		else
		{
			SetTableValue(x + 4, WiFi.getHostname());
			name = "A/P Name";
		}
		lv_table_set_cell_value(_table, x + 4, 0, name);

		// Connection count
		SetTableValue(x + 5, std::to_string(_webPortal.GetConnectCount() - 1).c_str());

		// ***** Refresh Ntrip Servers *****
		lv_obj_t *backup = _table; // Save the current table
		_table = _ntripTable;	   // Restore the NTRIP table

		SetNtripData(1, _ntripServer0);
		SetNtripData(2, _ntripServer1);
		SetNtripData(3, _ntripServer2);

		_table = backup;
	}

	///////////////////////////////////////////////////////////////////////////
	// Set the NTRIP data in the table for one ntrip server
	void SetNtripData(int column, NTRIPServer &svr)
	{
		const int max_chars = 7; // Maximum characters to display in the status
		std::string text = svr.GetStatus();
		if (text.length() > max_chars)
			text = text.substr(0, max_chars) + "...";
		SetTableString(1, text, column);
		SetTableShort(2, svr.GetReconnects(), column);
		SetTableShort(3, svr.GetPacketsSent(), column);
		SetTableShort(4, svr.GetTotalTimeouts(), column);
	}

	///////////////////////////////////////////////////////////////////////////
	// Return word between second last and last '.' (Domain name)
	std::string ShortServerName(const std::string &name)
	{
		std::string s = name;
		size_t lastDot = name.find_last_of('.');
		if (lastDot != std::string::npos)
		{
			size_t secondLastDot = name.find_last_of('.', lastDot - 1);
			if (secondLastDot == std::string::npos)
				s = name.substr(0, lastDot); // Only one dot found, return up to the last dot
			else
				s = name.substr(secondLastDot + 1, lastDot - secondLastDot - 1); // Return the part between the two dots
		}
		// Now trim the remainder with '...'
		return s;
	}
};
