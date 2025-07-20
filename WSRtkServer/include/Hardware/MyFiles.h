#pragma once

#include "FS.h"
#include "SPIFFS.h"
#include "HandyLog.h"
#include "LogFileSummary.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

///////////////////////////////////////////////////////////////////////////////
// File access routines
class MyFiles
{
	int _logLength = -1;		 // Length of the log file
	//fs::File _fsLog;			 // Log file
	SemaphoreHandle_t _mutexLog; // Thread safe access to writing logs
	SemaphoreHandle_t _mutex;	 // Thread safe access
	bool _flashOk = false;		 // Flash system access working
public:
	///////////////////////////////////////////////////////////////////////////
	// Setup the file system
	void SetupFlash()
	{
		// Setup mutex
		_mutex = xSemaphoreCreateMutex();
		_mutexLog = xSemaphoreCreateMutex();
		if (_mutex == NULL)
			Logln("*** FLASH ERROR : Failed to create flash FILE mutex\n");
		else
			Logln("Flash : File Mutex Created");

		// Check if the file system is mounted
		if (SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
		{
			Logln("SPIFFS Mount Failed");
			_flashOk = true;
			StartupComplete();
		}
		else
		{
			Logln("*** FLASH ERROR : SPIFFS Mount Failed");
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// @brief Called when the SPIFFS file system is started to list drive contents
	void StartupComplete()
	{
		// Dump the file system contents
		Logln("Drive contents");
		auto files = GetAllFilesSorted();
		if (files.empty())
		{
			Logln("\tNo files found in SPIFFS");
		}
		else
		{
			for (const auto &file : files)
			{
				Logf("\t %9d - %s", file.Size, file.Path.c_str());
			}
		}

		// Check the usage
		size_t totalBytes = SPIFFS.totalBytes();
		size_t usedBytes = SPIFFS.usedBytes();
		size_t freeBytes = totalBytes - usedBytes;
		Logf("\tUsed :%8u bytes", usedBytes);
		Logf("\tFree :%8u bytes", freeBytes);
		Logf("\tTotal:%8u bytes", totalBytes);

		// Record this startup
		auto file = SPIFFS.open(BOOT_LOG_FILENAME, FILE_APPEND);
		if (file)
		{
			file.printf("%s - %s\n", _handyTime.LongString().c_str(), APP_VERSION);
			file.close();
		}
		else
		{
			Logf("E207 - Failed to %s file for appending", BOOT_LOG_FILENAME);
			return;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// @brief Get a sorted list of all files in the SPIFFS file system.
	/// @return A vector of fs::File objects sorted by their path.
	std::vector<LogFileSummary> GetAllFilesSorted()
	{
		std::vector<LogFileSummary> files;
		auto root = SPIFFS.open("/");
		auto file = root.openNextFile();
		while (file)
		{
			// Note. If a file is modified during this stage, it will be duplicated in the list

			auto it = std::find_if(files.begin(), files.end(),
								   [&](const LogFileSummary &log)
								   { return strcmp(log.Path.c_str(), file.path()) == 0; });
			if (it == files.end()) // Only add if not already in the list
			{
				files.push_back(LogFileSummary(file, false));
			}
			file = root.openNextFile();
		}

		// Sort the list of files by their path
		std::sort(files.begin(), files.end(), [](const LogFileSummary &a, const LogFileSummary &b)
				  { return strcmp(a.Path.c_str(), b.Path.c_str()) < 0; });
		return files;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// @brief Write a message to the file system.
	bool WriteFile(const char *path, const char *message)
	{
		Logf("Writing file: %s -> '%s'", path, message);
		bool error = true;
		if (xSemaphoreTake(_mutex, portMAX_DELAY))
		{
			fs::File file = SPIFFS.open(path, FILE_WRITE);
			if (!file)
			{
				Logln("- failed to open file for writing");
				xSemaphoreGive(_mutex);
				return false;
			}

			error = file.print(message);
			Logln(error ? "- file written" : "- write failed");

			file.close();
			xSemaphoreGive(_mutex);
		}
		return error;
	}

	void AppendFile(const char *path, const char *message)
	{
		Logf("Appending to file: %s -> '%s'", path, message);
		if (xSemaphoreTake(_mutex, portMAX_DELAY))
		{
			fs::File file = SPIFFS.open(path, FILE_APPEND);
			if (!file)
			{
				Logln("- failed to open file for appending");
				xSemaphoreGive(_mutex);
				return;
			}
			if (file.print(message))
			{
				Logln("- message appended");
			}
			else
			{
				Logln("- append failed");
			}
			file.close();
			xSemaphoreGive(_mutex);
		}
	}

	bool ReadFile(const char *path, std::string &text, int maxLength = 256)
	{
		Logf("Reading file: %s", path);
		if (xSemaphoreTake(_mutex, portMAX_DELAY))
		{
			fs::File file = SPIFFS.open(path);
			if (!file || file.isDirectory())
			{
				Logln("- failed to open file for reading");
				xSemaphoreGive(_mutex);
				return false;
			}
			Serial.println("- read from file:");
			while (file.available())
			{
				char ch = static_cast<char>(file.read());
				if (ch == '\0')
				{
					Logln("- read NULL character");
					break;
				}
				text += ch;
				if (text.length() > maxLength)
				{
					Logf("- read %d bytes is greater than ", text.length(), maxLength);
					break;
				}
			}
			file.close();
			xSemaphoreGive(_mutex);
		}
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// @brief Load the string from the file system.
	/// @param readText Value to populate with the read text.
	/// @param path Location of the file to read.
	/// @note If the file does not exist, readText will not be set
	/// @param maxLength Maximum length of the string to read.
	void LoadString(std::string &readText, const char *path, int maxLength = 256)
	{
		std::string text;
		if (ReadFile(path, text))
		{
			Logln(StringPrintf(" - Read config '%s'", text.c_str()).c_str());
			readText.assign(text);
		}
		else
		{
			Logln(StringPrintf(" - E742 - Cannot read saved Server setting '%s'", path).c_str());
		}
	}
	std::string LoadString(const char *path, int maxLength = 256)
	{
		std::string text;
		if (ReadFile(path, text))
		{
			Logln(StringPrintf(" - Read config '%s'", text.c_str()).c_str());
			return text;
		}
		else
		{
			Logln(StringPrintf(" - E742 - Cannot read saved Server setting '%s'", path).c_str());
			return std::string();
		}
	}

	// ////////////////////////////////////////////////////////////////////////////////
	// /// @brief Close the log file if it is open.
	// void CloseLogFile(const char *closeMessage)
	// {
	// 	if (_fsLog)
	// 	{
	// 		if (closeMessage)
	// 			_fsLog.println(closeMessage);
	// 		_fsLog.close();
	// 		Serial.printf("Closing log file '%s'", _fsLog.path());
	// 	}
	// 	else
	// 	{
	// 		Serial.println("No log file to close");
	// 	}
	// 	_logLength = -1; // Reset the log length
	// }
};
