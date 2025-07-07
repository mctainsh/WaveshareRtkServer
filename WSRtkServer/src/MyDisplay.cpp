// Handy RGB565 colour picker at https://chrishewett.com/blog/true-rgb565-colour-picker/
// Image converter for sprite http://www.rinkydinkelectronics.com/t_imageconverter565.php

#include "MyDisplay.h"

#include "Global.h"
#include "HandyString.h"
#include "Hardware/MyFiles.h"
#include "GpsParser.h"

#include "HandyLog.h"
#include "History.h"

#include <iostream>
#include <sstream>
#include <string>

#include <iostream>
#include <sstream>
#include <string>
#include <NTRIPServer.h>
#include <WiFiManager.h>

// Font size 4 with 4 rows
#define R1F4 20
#define R2F4 50
#define R3F4 80
#define R4F4 110
#define R5F4 140

static const int F4_ROWS[] = {R1F4, R2F4, R3F4, R4F4, R5F4};
static const int F4_ROW_COUNT = sizeof(F4_ROWS) / sizeof(F4_ROWS[0]);

// Columns
#define COL1 5
#define COL2_P0 60
#define COL2_P0_W (TFT_HEIGHT - COL2_P0 - 5)

// Three column display on page 4
#define COL2_P4 60
#define COL3_P4 190
#define COL_W_P4 128

extern MyFiles _myFiles;
extern NTRIPServer _ntripServer0;
extern NTRIPServer _ntripServer1;
extern NTRIPServer _ntripServer2;
extern GpsParser _gpsParser;
extern WiFiManager _wifiManager;
extern History _history;

////////////////////////////////////////////////////////////////////////
// Setup this display
void MyDisplay::Setup()
{
	// Setup screen
	Logln("TFT Init");
	RefreshScreen();
}

// ************************************************************************//
// Page 0
// ************************************************************************//
void MyDisplay::Animate()
{
}

///////////////////////////////////////////////////////////////////////////
// Draw the GPS connected tick box
// void MyDisplay::SetGpsConnected(bool connected)
// {
// 	if (_gpsConnected == connected)
// 		return;
// 	_gpsConnected = connected;
// //	//_graphics.SetGpsConnected(_gpsConnected);
// }

// ************************************************************************//
// Page 3 - System detail
// ************************************************************************//
void MyDisplay::DisplayTime(unsigned long mil)
{
	// Uptime
	if (_currentPage == 0)
	{
		uint32_t t = mil/ 1000;
		std::string uptime = StringPrintf(":%02d", t % 60);
		t /= 60;
		uptime = StringPrintf(":%02d", t % 60) + uptime;
		t /= 60;
		uptime = StringPrintf("%02d", t % 24) + uptime;
		t /= 24;
		uptime = StringPrintf("%dd ", t) + uptime;
		DrawML(uptime.c_str(), COL2_P0, R3F4, COL2_P0_W, 4);
	}
}

void MyDisplay::SetPerformance(std::string performance)
{
	_performance = performance;
	if (_currentPage != 0)
		return;
	DrawML(performance.c_str(), COL2_P0, R5F4, COL2_P0_W, 4);
	return;
}

void MyDisplay::UpdateGpsStarts(bool restart, bool reinitialize)
{
	if (restart)
		_gpsResetCount++;
	if (reinitialize)
		_gpsReinitialize++;
	if (_currentPage == 2)
		DrawML(StringPrintf("R : %d  I : %d", _gpsResetCount, _gpsReinitialize).c_str(), COL2_P0, R2F4, COL2_P0_W, 4);
}
void MyDisplay::IncrementGpsPackets()
{
	SetValue(0, (_gpsMsgCount + 1), &_gpsMsgCount, COL2_P0, R5F4, COL2_P0_W, 4);
}

/////////////////////////////////////////////////////////////////////////////
// Depends on page currently shown
void MyDisplay::ActionButton()
{
	switch (_currentPage)
	{
	// Save current location
	case 2:
		break;

		// Reset GPS
		// case 3:
		//	Serial2.println("freset");
		//	break;
	}

	RefreshScreen();
}

///////////////////////////////////////////////////////////////////////////
// Move to the next page
void MyDisplay::NextPage()
{
	_currentPage++;
	if (_currentPage > 7)
		_currentPage = 0;
	Logf("Switch to page %d", _currentPage);
	RefreshScreen();
}

void MyDisplay::RefreshWiFiState()
{
	auto status = WiFi.status();
	//_graphics.SetWebStatus(status);
	if (_currentPage != 0)
		return;
	if (status == WL_CONNECTED)
	{
		DrawML(WiFi.localIP().toString().c_str(), COL2_P0, R1F4, COL2_P0_W, 4);
	}
	else
	{
		DrawML(WifiStatus(status), COL2_P0, R1F4, COL2_P0_W, 4);
		DrawML("X-192.168.4.1", COL2_P0, R3F4, COL2_P0_W, 4);
		DrawML(WiFi.getHostname(), COL2_P0, R4F4, COL2_P0_W, 4);
		DrawML(AP_PASSWORD, COL2_P0, R5F4, COL2_P0_W, 4);
	}
}

NTRIPServer *GetServer(int index)
{
	switch (index)
	{
	case 0:
		return &_ntripServer0;
	case 1:
		return &_ntripServer1;
	default:
		return &_ntripServer2;
	}
}

///////////////////////////////////////////////////////////////////////////
// Refresh the RTK server status (Only display first two servers)
void MyDisplay::RefreshRtk(int index)
{
	auto pServer = GetServer(index);
	//_graphics.SetRtkStatus(index, pServer->GetStatus());

	if (_currentPage != 1 || index > 1)
		return;
	int col = index == 0 ? COL2_P4 : COL3_P4;

	DrawML(pServer->GetStatus(), col, R1F4, COL_W_P4, 4);
	DrawMR(ToThousands(pServer->GetReconnects()).c_str(), col, R2F4, COL_W_P4, 4);
	DrawMR(ToThousands(pServer->GetPacketsSent()).c_str(), col, R3F4, COL_W_P4, 4);
	DrawMR(ToThousands(_history.MedianSendTime(pServer->GetIndex())).c_str(), col, R4F4, COL_W_P4, 4);
}

void MyDisplay::RefreshRtkLog()
{
	// if (4 > _currentPage || _currentPage > 6)
	// 	return;
	// NTRIPServer *pServer = GetServer(_currentPage - 4);
	// RefreshLog(pServer->GetLogHistory());
}
void MyDisplay::RefreshGpsLog()
{
	// if (_currentPage != 3)
	// 	return;
	// RefreshLog(_gpsParser.GetLogHistory());
}
void MyDisplay::RefreshLog(const std::vector<std::string> &log)
{
	// // Clear the background
	// //_tft.fillRect(0, R1F4, TFT_HEIGHT, TFT_WIDTH, TFT_BLACK);

	// //_tft.setCursor(0, R1F4);
	// //_tft.setTextColor(TFT_GREEN, TFT_BLACK);
	// //_tft.setTextDatum(TL_DATUM);
	// //_tft.setTextFont(1);

	// int rows = 0;
	// for (auto it = log.rbegin(); it != log.rend(); ++it)
	// {
	// 	std::string truncated = it->c_str();
	// 	truncated.erase(0, truncated.find_first_not_of("0 "));
	// 	truncated = Replace(truncated, "\t", "    ");
	// 	truncated = Replace(truncated, "\r", "");
	// 	truncated = Replace(truncated, "\n", "-");
	// 	//_tft.println(truncated.substr(0, 53).c_str());
	// 	if (rows++ > 16)
	// 		break;
	// }
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Draw everything back on the screen
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void MyDisplay::RefreshScreen()
{
	// const char *title = "Unknown";
	// //_tft.setTextDatum(TL_DATUM);
	// _fg = TFT_WHITE;

	// switch (_currentPage)
	// {
	// case 0:
	// 	_bg = 0x07eb;
	// 	_fg = TFT_BLACK;
	// 	//_tft.fillScreen(_bg);
	// 	title = "  0 - General";

	// 	DrawLabel("Wi-Fi", COL1, R1F4, 2);
	// 	DrawLabel("SSID", COL1, R2F4, 2);
	// 	DrawLabel("Up time", COL1, R3F4, 2);
	// 	DrawLabel("Hostname", COL1, R4F4, 2);
	// 	DrawLabel("Mm T Wi", COL1, R5F4, 2);

	// 	DrawML(WiFi.SSID().c_str(), COL2_P0, R2F4, COL2_P0_W, 4);
	// 	//DrawML(_wifiManager.getWiFiSSID(false).c_str(), COL2_P0, R2F4, COL2_P0_W, 4);
	// 	DrawML(StringPrintf("%s.local", _mdnsHostName.c_str()).c_str(), COL2_P0, R4F4, COL2_P0_W, 4);
	// 	DrawML(_performance.c_str(), COL2_P0, R5F4, COL2_P0_W, 4);

	// 	break;
	// case 1:
	// 	_bg = TFT_ORANGE;
	// 	_fg = TFT_BLACK;
	// 	//_tft.fillScreen(_bg);
	// 	title = "  1 - RTK Server";
	// 	DrawLabel("State", COL1, R1F4, 2);
	// 	DrawLabel("Reconnects", COL1, R2F4, 2);
	// 	DrawLabel("Sends", COL1, R3F4, 2);
	// 	DrawLabel("us", COL1, R4F4, 2);
	// 	break;
	// case 2:
	// 	//_bg = TFT_RED;
	// 	//_tft.fillScreen(_bg);
	// 	title = "  2 - GPS State";

	// 	DrawLabel("Type", COL1, R1F4, 2);
	// 	DrawLabel("Resets", COL1, R2F4, 2);
	// 	DrawLabel("Serial #", COL1, R3F4, 2);
	// 	DrawLabel("Firmware", COL1, R4F4, 2);

	// 	DrawML(_gpsParser.GetCommandQueue().GetDeviceType().c_str(), COL2_P0, R1F4, COL2_P0_W, 4);
	// 	DrawML(_gpsParser.GetCommandQueue().GetDeviceSerial().c_str(), COL2_P0, R3F4, COL2_P0_W, 4);
	// 	DrawML(_gpsParser.GetCommandQueue().GetDeviceFirmware().c_str(), COL2_P0, R4F4, COL2_P0_W, 4);

	// 	break;
	// case 3:
	// 	_bg = TFT_BLACK;
	// 	//_tft.fillScreen(_bg);
	// 	//_tft.fillScreen(TFT_BLACK);
	// 	title = "  3 - GPS Log";
	// 	break;

	// case 4:
	// 	_bg = TFT_BLACK;
	// 	//_tft.fillScreen(_bg);
	// 	//_tft.fillScreen(TFT_BLACK);
	// 	title = StringPrintf("  4 - %s", _ntripServer0.GetAddress().c_str()).c_str();
	// 	break;

	// case 5:
	// 	_bg = TFT_BLACK;
	// 	//_tft.fillScreen(_bg);
	// 	title = StringPrintf("  5 - %s", _ntripServer1.GetAddress().c_str()).c_str();
	// 	break;

	// case 6:
	// 	_bg = TFT_BLACK;
	// 	//_tft.fillScreen(_bg);
	// 	title = StringPrintf("  6 - %s", _ntripServer2.GetAddress().c_str()).c_str();
	// 	break;
	// case 7:
	// 	_bg = 0xa51f;
	// 	_fg = TFT_BLACK;
	// 	//_tft.fillScreen(_bg);
	// 	title = "  6 - Key";
	// 	DrawKeyLine(R1F4, 4);
	// 	DrawKeyLine(R2F4, 3);
	// 	DrawKeyLine(R3F4, 2);
	// 	DrawKeyLine(R4F4, 1);
	// 	DrawKeyLine(R5F4, 0);
	// 	DrawLabel("Wi-Fi ---(Reset)", COL1, R1F4, 4);
	// 	DrawLabel("GPS", COL1, R2F4, 4);
	// 	DrawLabel("RTK Server 1 ", COL1, R3F4, 4);
	// 	DrawLabel("RTK Server 3 ", COL1, R5F4, 4);
	// 	DrawLabel("RTK Server 2 ", COL1, R4F4, 4);
	// }
	// DrawML(title, 20, 0, 200, 2);

	// // Redraw
	// //_graphics.SetGpsConnected(_gpsConnected);

	// UpdateGpsStarts(false, false);
	// _gpsMsgCount--;
	// IncrementGpsPackets();

	// RefreshWiFiState();

	// RefreshRtk(0);
	// RefreshRtk(1);
	// RefreshGpsLog();
	// RefreshRtkLog();
}

/////////////////////////////////////////////////////////////////////////////
template <typename T>
void MyDisplay::SetValue(int page, T n, T *pMember, int32_t x, int32_t y, int width, uint8_t font)
{
	if (*pMember == n)
		return;
	*pMember = n;

	if (page != _currentPage)
		return;

	std::string text;
	if (std::is_same<T, int>::value)
	{
		text = ToThousands(*pMember);
	}
	else
	{
		std::ostringstream oss;
		oss << *pMember;
		text = oss.str();
	}
	DrawCell(text.c_str(), x, y, width, font);
}

void MyDisplay::SetCell(std::string text, int page, int row, uint8_t datum)
{
	if (page != _currentPage)
		return;

	int y = (0 <= row && row < F4_ROW_COUNT) ? F4_ROWS[row] : R1F4;
	DrawML(text.c_str(), COL2_P0, y, COL2_P0_W, 4);
}

template <typename T>
void MyDisplay::SetValueFormatted(int page, T n, T *pMember, const std::string text, int32_t x, int32_t y, int width, uint8_t font)
{
	if (*pMember == n)
		return;
	*pMember = n;
	if (_currentPage != page)
		return;
	DrawCell(text.c_str(), x, y, width, font);
}

///////////////////////////////////////////////////////////////////////////
// Draw line from text to top row tick position
void MyDisplay::DrawKeyLine(int y, int tick)
{
	y += 10;
	//int box = TFT_HEIGHT - (SPACE * tick) - CW / 2;
	//_tft.drawLine(50, y, box, y, TFT_BLACK);
	//_tft.drawLine(box, y, box, CH, TFT_BLACK);
}

///////////////////////////////////////////////////////////////////////////
// NOTE: Font 6 is only numbers
void MyDisplay::DrawCell(const char *pstr, int32_t x, int32_t y, int width, uint8_t font, uint16_t fgColour, uint16_t bgColour, uint8_t datum)
{
	// // Fill background
	// int height = //_tft.fontHeight(font);
	// //_tft.fillRoundRect(x, y, width, height + 3, 5, bgColour);

	// // Draw text
	// //_tft.setFreeFont(&FreeMono18pt7b);
	// //_tft.setTextColor(fgColour, bgColour, true);
	// //_tft.setTextDatum(datum);
	// if (datum == ML_DATUM)
	// 	//_tft.drawString(pstr, x + 2, y + (height / 2) + 2, font);
	// else if (datum == MR_DATUM)
	// 	//_tft.drawString(pstr, x + width - 2, y + (height / 2) + 2, font);
	// else
	// 	//_tft.drawString(pstr, x + width / 2, y + (height / 2) + 2, font);

	// // Frame it
	// //_tft.drawRoundRect(x, y, width, height + 3, 5, TFT_YELLOW);
}
void MyDisplay::DrawML(const char *pstr, int32_t x, int32_t y, int width, uint8_t font, uint16_t fgColour, uint16_t bgColour)
{
	//DrawCell(pstr, x, y, width, font, fgColour, bgColour, ML_DATUM);
}
void MyDisplay::DrawMR(const char *pstr, int32_t x, int32_t y, int width, uint8_t font, uint16_t fgColour, uint16_t bgColour)
{
	//DrawCell(pstr, x, y, width, font, fgColour, bgColour, MR_DATUM);
}

///////////////////////////////////////////////////////////////////////////
// NOTE: Font 6 is only numbers
void MyDisplay::DrawLabel(const char *pstr, int32_t x, int32_t y, uint8_t font)
{
	// Fill background
	//int height = //_tft.fontHeight(font);
	////_tft.fillRoundRect(x, y, width, height + 3, 5, bgColour);

	// Draw text
	//_tft.setFreeFont(&FreeMono18pt7b);
	//_tft.setTextColor(_fg, _bg, true);
	//_tft.setTextDatum(ML_DATUM);
	//_tft.drawString(pstr, x, y + (height / 2) + 2, font);

	// Frame it
	////_tft.drawRoundRect(x, y, width, height + 3, 5, TFT_YELLOW);
}
