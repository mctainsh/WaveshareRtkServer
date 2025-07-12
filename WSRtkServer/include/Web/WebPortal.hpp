#pragma once

#include <WiFi.h>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
/// @brief Class manages the web pages displayed in the device.
class WebPortal
{
private:
	int _connectCount = 0; // Number of time we have connected
	int _webLoopTime = 0;		   // Use to prevent the PortalProcessing to take up too much processing time

public:
	int GetConnectCount() const { return _connectCount; } // Get the number of times we have connected

	void Setup();
	void OnConnected();

	void Loop( unsigned long t);

private:
	void IndexHtml();
	void SettingsHtml();
	void FileManagerHtml();
	void SdCardManagerHtml();

	void OnBindServerCallback();
	void ShowStatusHtml();
	void GraphHtml() const;
	void GraphTemperature() const;
	void GraphArray(WiFiClient &client, std::string divId, std::string title, const char *pBytes, int length) const;
	void HtmlLog(const char *title, const std::vector<std::string> &log) const;
};