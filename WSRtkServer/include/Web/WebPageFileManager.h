#pragma once

#include "Web\WebPageWrapper.h"
#include "HandyLog.h"
#include "Hardware/MyFiles.h"

extern MyFiles _myFiles;
extern SDFile _sdFile;

///////////////////////////////////////////////////////////////////////////////
// Fancy HTML pages for the web portal
class WebPageFileManager : public WebPageWrapper
{
private:
	bool _sdCard = false; // True if the SD card is used, false for SPIFFS
public:
	WebPageFileManager(WiFiClient &c, bool sdCard) : WebPageWrapper(c)
	{
		_sdCard = sdCard;
		if (_sdCard)
			Logln("WebPageFileManager: SD Card");
		else
			Logln("WebPageFileManager: SPIFFS");
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @brief Confirm the GPS Reset
	void ShowHtml()
	{
		Logln("Show Files Html");

		// Return the file if requested with 'RequestUrl'
		if (ReturnFile())
			return;

		DeleteFile();

		AddPageHeader(_wifiManager.server->uri().c_str());

		_client.println("<style>.flex-row { display: flex;flex-wrap: wrap;gap: 1rem;}.flex-item "
						"{flex: 1 1 300px; min-width: 300px;background-color: #0001;box-sizing: border-box;}</style>");

		// Add the form for the caster 1
		_client.printf("<h3 class='mt-4'>File Manager</h3>"
					   "<pre style='font-family:Courier New, Courier, monospace; border:none;'>");

		auto files = _sdCard ? _sdFile.GetAllFilesSorted() : _myFiles.GetAllFilesSorted();
		// auto path = _sdCard ? _sdFile.GetState() : _myFiles.GetState();
		for (const auto &file : files)
		{
			const int PADDED_SIZE = 30;
			std::string padding;
			if (file.Path.length() < PADDED_SIZE)
				padding = std::string(PADDED_SIZE - file.Path.length(), '.');

			//<a href="SystemTools/TextViewer?RequestUrl=system%2Fviewlog%2FiSurvMonitor_2025-06-27T00.00.10">iSurvMonitor 2025-06-27 00.00.10</a>    20kb<a href="system/downloadlog?F=iSurvMonitor_2025-06-27T00.00.10&amp;" download="iSurvMonitor 2025-06-27 00.00.10.txt"><!--!-->📥</a>
			if (file.IsCurrentLog)
				_client.printf("%s %s %7d<br>", file.Path.c_str(), padding.c_str(), file.Size);
			else
				_client.printf("<a href='?RequestUrl=%s' download='%s'>%s</a> %s %7d <a href='?delete=%s'><i class='bi bi-trash3-fill'></i></a><br>",
							   file.Path.c_str(), file.Path.c_str(), file.Path.c_str(), padding.c_str(), file.Size,
							   file.Path.c_str());
		}
		_client.println("</pre>");

		AddPageFooter();
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @brief Return the file if requested with 'RequestUrl'
	bool ReturnFile()
	{
		Logf("WebPageFileManager:ReturnFile(%s)", _wifiManager.server->uri().c_str());
		if (!_wifiManager.server->hasArg("RequestUrl"))
			return false;

		auto filePath = _wifiManager.server->arg("RequestUrl");
		Logf("RequestUrl SD:%d Path:%s", _sdCard, filePath.c_str());

		// Get the file
		fs::File file;
		if (_sdCard)
			file = SD_MMC.open(filePath.c_str());
		else
			file = SPIFFS.open(filePath.c_str());
		if (file)
		{
			// Optional: send HTTP headers if needed
			// client.println("POST /upload HTTP/1.1");
			// client.println("Host: your.server.com");
			// client.println("Content-Type: text/plain");
			// client.print("Content-Length: ");
			// client.println(file.size());
			// client.println(); // End of headers

			// Stream file content
			uint8_t buffer[512];
			while (file.available())
			{
				size_t len = file.read(buffer, sizeof(buffer));
				_client.write(buffer, len);
			}
			file.close();
		}
		else
		{
			Logln("\t*** ERROR : Failed to read file");
			_client.println("File not found");
		}
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////
	/// @brief Delete a file if requested with 'delete'
	void DeleteFile()
	{
		if (!_wifiManager.server->hasArg("delete"))
			return;

		// Get the file path to delete
		auto filePath = _wifiManager.server->arg("delete");
		Logf("Deleting file %s", filePath.c_str());
		bool deleted;
		if (_sdCard)
			deleted = SD_MMC.remove(filePath.c_str());
		else
			deleted = SPIFFS.remove(filePath.c_str());
		if (deleted)
			Logln("\tFile deleted");
		else
			Logln("\t *** ERROR : Failed to delete file");
	}
};
