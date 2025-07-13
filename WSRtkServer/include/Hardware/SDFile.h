#pragma once

#include "FS.h"
#include "SD_MMC.h"
#include "HandyString.h"
#include "HandyLog.h"
#include "LogFileSummary.h"

///////////////////////////////////////////////////////////////////////////////
// Panel containing a label and a value
class SDFile
{
private:
	bool _isMounted = false;	 // True if mounted
	std::string _error;			 // Error state of the SD_MMC card
	std::string _cardType;		 // Type of SD card
	uint64_t _cardSize = 0;		 // Size of the SD_MMC card in bytes
	int _logLength = -1;		 // Log Length of the log file
	fs::File _fsLog;			 // Log file
	SemaphoreHandle_t _mutexLog; // Thread safe access to writing logs

public:
	bool LogStarted() { return _logLength > 0; } // True if log file is started

	///////////////////////////////////////////////////////////////////////////
	// Setup SD_MMC card
	// This function initializes the SD_MMC card and prints its type and size.
	void Setup()
	{
		// Default pins for ESP-S3
		// Warning: ESP32-S3-WROOM-2 is using most of the default GPIOs (33-37) to interface with on-board OPI flash.
		//   If the SD_MMC is initialized with default pins it will result in rebooting loop - please
		//   reassign the pins elsewhere using the mentioned command `setPins`.
		// Note: ESP32-S3-WROOM-1 does not have GPIO 33 and 34 broken out.
		// Note: if it's ok to use default pins, you do not need to call the setPins
		int SD_MMC_CLK = 11;
		int SD_MMC_CMD = 10;
		int SD_MMC_D0 = 9;

		_mutexLog = xSemaphoreCreateMutex();
		if (!SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0))
		{
			_error = "Failed to set SD_MMC pins";
			Logln(_error.c_str());
			return;
		}

		if (!SD_MMC.begin("/sdcard", true))
		{
			_error = "SD_MMC Mount Failed";
			Logln(_error.c_str());
			return;
		}
		uint8_t cardType = SD_MMC.cardType();

		if (cardType == CARD_NONE)
		{
			_error = "No SD_MMC card attached";
			Logln(_error.c_str());
			return;
		}
		_isMounted = true;

		if (cardType == CARD_MMC)
			_cardType = "MMC";
		else if (cardType == CARD_SD)
			_cardType = "SDSC";
		else if (cardType == CARD_SDHC)
			_cardType = "SDHC";
		else
			_cardType = std::to_string(cardType);
		Logf("SD_MMC Card Type:%s", _cardType.c_str());

		_cardSize = SD_MMC.cardSize();
		Logf("SD_MMC Card Size: %lluMB", _cardSize / (1024 * 1024));

		CreateDir(SD_MMC, "/logs");
		// ListDir(SD_MMC, "/", 0);
		// CreateDir(SD_MMC, "/mydir");
		// ListDir(SD_MMC, "/", 0);
		// RemoveDir(SD_MMC, "/mydir");
		// ListDir(SD_MMC, "/", 2);
		// WriteFile(SD_MMC, "/hello.txt", "Hello ");
		// AppendFile(SD_MMC, "/hello.txt", "World!");
		// ReadFile(SD_MMC, "/hello.txt");
		// DeleteFile(SD_MMC, "/foo.txt");
		// RenameFile(SD_MMC, "/hello.txt", "/foo.txt");
		// ReadFile(SD_MMC, "/foo.txt");

		TestFileIO(SD_MMC, "/test.txt");
		Logf("Total space: %lluMB", SD_MMC.totalBytes() / (1024 * 1024));
		Logf("Used space: %lluMB", SD_MMC.usedBytes() / (1024 * 1024));
	}

	// ////////////////////////////////////////////////////////////////////////////////
	// /// @brief Get a sorted list of all files in the SD_MMC file system.
	// /// @return A vector of fs::File objects sorted by their path.
	std::vector<LogFileSummary> GetAllFilesSorted()
	{
		const char *logPath = _fsLog ? _fsLog.path() : "";
		std::vector<LogFileSummary> files;

		listAllFiles(SD_MMC, "/", files, logPath);

		// Sort the list of files by their path
		std::sort(files.begin(), files.end(), [](const LogFileSummary &a, const LogFileSummary &b)
				  { return strcmp(a.Path.c_str(), b.Path.c_str()) < 0; });
		return files;
	}

	///////////////////////////////////////////////////////////////////////////
	// Recursively list all files
	void listAllFiles(fs::FS &fs, const char *dirName, std::vector<LogFileSummary> &files, const char *logPath)
	{
		File root = fs.open(dirName);
		if (!root || !root.isDirectory())
		{
			Serial.printf("Failed to open directory: %s\n", dirName);
			return;
		}

		File file = root.openNextFile();
		while (file)
		{
			if (file.isDirectory())
			{
				// Skip system folders
				if (strcmp(file.path(), "/System Volume Information") != 0)
				{
					// Recurse into subdirectory
					listAllFiles(fs, file.path(), files, logPath);
				}
			}
			else
			{
				// Avoid duplicates
				auto it = std::find_if(files.begin(), files.end(),
									   [&](const LogFileSummary &log)
									   {
										   return strcmp(log.Path.c_str(), file.path()) == 0;
									   });

				// If not found, add to the list
				if (it == files.end())
					files.push_back(LogFileSummary(file, strcmp(file.path(), logPath) == 0));
			}
			file = root.openNextFile();
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Get the card type or error state
	std::string GetState()
	{
		if (!_isMounted)
			return "SD_MMC not mounted";
		if (_error.length() > 0)
			return _error;
		return _cardType;
	}

	///////////////////////////////////////////////////////////////////////////
	// Get the card size in MB
	std::string GetDriveSpace()
	{
		if (!_isMounted)
			return "N/A";
		return MakeKbPercent(SD_MMC.usedBytes(), SD_MMC.totalBytes(), MEGAB);
	}

	///////////////////////////////////////////////////////////////////////////
	// List directory contents
	// This function lists the contents of a directory up to a specified number of levels deep.
	void ListDir(fs::FS &fs, const char *dirname, uint8_t levels)
	{
		Logf("Listing directory: %s", dirname);
		if (!_isMounted)
		{
			Logln("SD_MMC not mounted");
			return;
		}

		File root = fs.open(dirname);
		if (!root)
		{
			Logln("Failed to open directory");
			return;
		}
		if (!root.isDirectory())
		{
			Logln("Not a directory");
			return;
		}

		File file = root.openNextFile();
		while (file)
		{
			if (file.isDirectory())
			{
				Serial.print("  DIR : ");
				Logln(file.name());
				if (levels)
				{
					ListDir(fs, file.path(), levels - 1);
				}
			}
			else
			{
				Logf("  FILE: %s  SIZE: %lu", file.name(), file.size());
			}
			file = root.openNextFile();
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Create a directory
	// This function creates a directory at the specified path.
	void CreateDir(fs::FS &fs, const char *path)
	{
		Logf("Creating Dir: %s", path);
		if (!_isMounted)
		{
			Logln("SD_MMC not mounted");
			return;
		}
		if (!fs.mkdir(path))
			Logln("mkdir failed");
	}

	///////////////////////////////////////////////////////////////////////////
	// Remove a directory
	// This function removes a directory at the specified path.
	void RemoveDir(fs::FS &fs, const char *path)
	{
		Logf("Removing Dir: %s", path);
		if (!_isMounted)
		{
			Logln("SD_MMC not mounted");
			return;
		}
		if (!fs.rmdir(path))
			Logln("rmdir failed");
	}

	///////////////////////////////////////////////////////////////////////////
	// Read a file
	// This function reads the contents of a file and prints it to the serial console.
	void ReadFile(fs::FS &fs, const char *path)
	{
		Logf("Reading file: %s", path);
		if (!_isMounted)
		{
			Logln("SD_MMC not mounted");
			return;
		}
		File file = fs.open(path);
		if (!file)
		{
			Logln("Failed to open file for reading");
			return;
		}

		Serial.print("Read from file: ");
		while (file.available())
		{
			Serial.write(file.read());
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Write a file
	// This function writes a message to a file at the specified path.
	void WriteFile(fs::FS &fs, const char *path, const char *message)
	{
		Logf("Writing file: %s", path);
		if (!_isMounted)
		{
			Logln("SD_MMC not mounted");
			return;
		}
		File file = fs.open(path, FILE_WRITE);
		if (!file)
		{
			Logln("Failed to open file for writing");
			return;
		}
		if (file.print(message))
		{
			Logln("File written");
		}
		else
		{
			Logln("Write failed");
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Append a message to a file
	// This function appends a message to a file at the specified path.
	void AppendFile(fs::FS &fs, const char *path, const char *message)
	{
		Logf("Appending to file: %s", path);
		if (!_isMounted)
		{
			Logln("SD_MMC not mounted");
			return;
		}

		File file = fs.open(path, FILE_APPEND);
		if (!file)
		{
			Logln("Failed to open file for appending");
			return;
		}
		if (!file.print(message))
			Logln("Append failed");
	}

	///////////////////////////////////////////////////////////////////////////
	// Rename a file
	// This function renames a file from path1 to path2.
	void RenameFile(fs::FS &fs, const char *path1, const char *path2)
	{
		Logf("Renaming file %s to %s", path1, path2);
		if (!_isMounted)
		{
			Logln("SD_MMC not mounted");
			return;
		}
		if (!fs.rename(path1, path2))
			Logln("Rename failed");
	}

	///////////////////////////////////////////////////////////////////////////
	// Delete a file
	// This function deletes a file at the specified path.
	void DeleteFile(fs::FS &fs, const char *path)
	{
		Logf("Deleting file: %s", path);
		if (!_isMounted)
		{
			Logln("SD_MMC not mounted");
			return;
		}
		if (!fs.remove(path))
			Logln("Delete failed");
	}

	///////////////////////////////////////////////////////////////////////////
	void TestFileIO(fs::FS &fs, const char *path)
	{
		File file = fs.open(path);
		static uint8_t buf[512];
		size_t len = 0;
		uint32_t start = millis();
		uint32_t end = start;
		if (file)
		{
			len = file.size();
			size_t flen = len;
			start = millis();
			while (len)
			{
				size_t toRead = len;
				if (toRead > 512)
				{
					toRead = 512;
				}
				file.read(buf, toRead);
				len -= toRead;
			}
			end = millis() - start;
			Logf("%u bytes read for %lu ms", flen, end);
			file.close();
		}
		else
		{
			Logln("Failed to open file for reading");
		}

		file = fs.open(path, FILE_WRITE);
		if (!file)
		{
			Logln("Failed to open file for writing");
			return;
		}

		size_t i;
		start = millis();
		for (i = 0; i < 2048; i++)
		{
			file.write(buf, 512);
		}
		end = millis() - start;
		Logf("%u bytes written for %lu ms", 2048 * 512, end);
		file.close();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// @brief Start a new logging file or open the existing one
	void StartLogFile(std::vector<std::string> *pMainLog)
	{
		if (!xSemaphoreTake(_mutexLog, portMAX_DELAY))
			return;

		_logLength = -1; // Reset the log length
		// Open the current file if already exists
		if (_fsLog)
		{
			_fsLog.println("**** CLOSING EXISTING FILE FOR ROLLOVER ****");
			_fsLog.close(); // Close the existing log file
		}

		// Remove old log files if too much drive is used
		Serial.print("DUMP SD Files\n");
		auto files = GetAllFilesSorted();

		// Remove non-log files from the list
		for (int i = files.size() - 1; i >= 0; i--)
			if (files[i].Path.find(LOG_FILE_PREFIX) != 0 || files[i].IsCurrentLog)
				files.erase(files.begin() + i);

		// If we have more than MAX_LOG_FILES, remove the oldest ones
		const int MAX_LOG_FILES = 50; // Maximum number of log files to keep
		int filesToRemove = files.size() - MAX_LOG_FILES;

		// Starting at the first item, remove files until we have less than 250kb available
		size_t totalFree = SD_MMC.totalBytes() - SD_MMC.usedBytes();
		for (const auto &file : files)
		{
			if( filesToRemove > 0)
			{
				filesToRemove--;
				
			}
			else 
			{
				if (totalFree > 1024 * 1024) // Keep at least 1MB free
					break;
			}

			// Remove the file
			Serial.printf("Removing old log file: %s (%d bytes)\n", file.Path.c_str(), file.Size);
			totalFree += file.Size;
			SD_MMC.remove(file.Path.c_str());
		}

		// Display the file we are working with
		for (const auto &file : files)
			Serial.printf("Log file: %s (%d bytes)\r\n", file.Path.c_str(), file.Size);

		// Open existing or create a new log file
		auto filename = LOG_FILE_PREFIX + _handyTime.FileSafe() + ".txt";
		Serial.printf("Log open file: %s\n", filename.c_str());
		if (SD_MMC.exists(filename.c_str()))
		{
			Serial.println("\tAPPENDING");
			_fsLog = SD_MMC.open(filename.c_str(), FILE_APPEND);
			_fsLog.println("**** APPENDING EXISTING FILE ****");
			_logLength = _fsLog.size(); // Get the size of the existing log file
		}
		if (!_fsLog)
		{
			Serial.println("\tCREATING");
			_fsLog = SD_MMC.open(filename.c_str(), FILE_WRITE);
			if (_fsLog)
				_logLength = 0;
			else
				Serial.println("\t*** ERROR : Failed to create Log.");
		}
		xSemaphoreGive(_mutexLog);

		// Copy the main log into the log file
		if (pMainLog != NULL)
			for (const auto &line : *pMainLog)
				AppendLog(line.c_str());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// @brief Save the string to the file system.
	void AppendLog(const char *message)
	{
		if (_logLength < 0)
			return;

		_logLength += strlen(message);

		if (_logLength > 100000)
		{
			std::vector<std::string> logHeader;
			logHeader.push_back(StringPrintf("***** Rolling over from log file, length %s", _fsLog.path()));
			StartLogFile(&logHeader);
		}

		if (!xSemaphoreTake(_mutexLog, portMAX_DELAY))
			return;

		// SD_MMC and Arduino File I/O do not throw exceptions, so try-catch is not needed.
		if (_fsLog)
		{
			_fsLog.println(message);
			_fsLog.flush();
		}
		xSemaphoreGive(_mutexLog);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// @brief Close the log file if it is open.
	void CloseLogFile()
	{
		if (_fsLog)
		{
			_fsLog.close();
			_logLength = -1; // Reset the log length
		}
	}
};
