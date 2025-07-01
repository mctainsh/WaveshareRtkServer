#pragma once

#include "FS.h"
#include "SD_MMC.h"

#ifdef CONFIG_IDF_TARGET_ESP32S3
// Default pins for ESP-S3
// Warning: ESP32-S3-WROOM-2 is using most of the default GPIOs (33-37) to interface with on-board OPI flash.
//   If the SD_MMC is initialized with default pins it will result in rebooting loop - please
//   reassign the pins elsewhere using the mentioned command `setPins`.
// Note: ESP32-S3-WROOM-1 does not have GPIO 33 and 34 broken out.
// Note: if it's ok to use default pins, you do not need to call the setPins
int clk = 11;
int cmd = 10;
int d0 = 9;

#endif

///////////////////////////////////////////////////////////////////////////////
// Panel containing a label and a value
class SDFile
{
private:
	bool _isMounted = false;
	std::string _error;
	std::string _cardType;
	uint64_t _cardSize = 0;

public:
	///////////////////////////////////////////////////////////////////////////
	// Setup SD_MMC card
	// This function initializes the SD_MMC card and prints its type and size.
	void Setup()
	{
		if (!SD_MMC.setPins(clk, cmd, d0))
		{
			_error = "Failed to set SD_MMC pins";
			Serial.println(_error.c_str());
			return;
		}

		if (!SD_MMC.begin("/sdcard", true))
		{
			_error = "SD_MMC Mount Failed";
			Serial.println(_error.c_str());
			return;
		}
		uint8_t cardType = SD_MMC.cardType();

		if (cardType == CARD_NONE)
		{
			_error = "No SD_MMC card attached";
			Serial.println(_error.c_str());
			return;
		}
		_isMounted = true;

		Serial.print("SD_MMC Card Type: ");
		if (cardType == CARD_MMC)
			_cardType = "MMC";
		else if (cardType == CARD_SD)
			_cardType = "SDSC";
		else if (cardType == CARD_SDHC)
			_cardType = "SDHC";
		else
			_cardType = std::to_string(cardType);
		Serial.printf(("SD_MMC Card Type:" + _cardType).c_str());

		_cardSize = SD_MMC.cardSize();
		Serial.printf("SD_MMC Card Size: %lluMB\n", _cardSize / (1024 * 1024));

		ListDir(SD_MMC, "/", 0);
		CreateDir(SD_MMC, "/mydir");
		ListDir(SD_MMC, "/", 0);
		RemoveDir(SD_MMC, "/mydir");
		ListDir(SD_MMC, "/", 2);
		WriteFile(SD_MMC, "/hello.txt", "Hello ");
		AppendFile(SD_MMC, "/hello.txt", "World!\n");
		ReadFile(SD_MMC, "/hello.txt");
		DeleteFile(SD_MMC, "/foo.txt");
		RenameFile(SD_MMC, "/hello.txt", "/foo.txt");
		ReadFile(SD_MMC, "/foo.txt");

		TestFileIO(SD_MMC, "/test.txt");
		Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
		Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
	}

	// ////////////////////////////////////////////////////////////////////////////////
	// /// @brief Get a sorted list of all files in the SPIFFS file system.
	// /// @return A vector of fs::File objects sorted by their path.
	// std::vector<LogFileSummary> GetAllFilesSorted()
	// {
	// 	const char *logPath = _fsLog ? _fsLog.path() : "";
	// 	std::vector<LogFileSummary> files;
	// 	auto root = SPIFFS.open("/");
	// 	auto file = root.openNextFile();
	// 	while (file)
	// 	{
	// 		// Note. If a file is modified during this stage, it will be duplicated in the list

	// 		auto it = std::find_if(files.begin(), files.end(),
	// 							   [&](const LogFileSummary &log)
	// 							   { return strcmp(log.Path.c_str(), file.path()) == 0; });
	// 		if (it == files.end()) // Only add if not already in the list
	// 		{
	// 			files.push_back(LogFileSummary(file, strcmp(file.path(), logPath) == 0));
	// 		}
	// 		file = root.openNextFile();
	// 	}

	// 	// Sort the list of files by their path
	// 	std::sort(files.begin(), files.end(), [](const LogFileSummary &a, const LogFileSummary &b)
	// 			  { return strcmp(a.Path.c_str(), b.Path.c_str()) < 0; });
	// 	return files;
	// }

	std::string GetState()
	{
		if (!_isMounted)
			return "SD_MMC not mounted";
		if (_error.length() > 0)
			return _error;
		return _cardType + ", " + std::to_string(_cardSize / (1024 * 1024)) + "MB";
	}

	std::string GetDriveSpace()
	{
		if (!_isMounted)
			return "N/A";
		auto free = SD_MMC.usedBytes();
		auto total = SD_MMC.totalBytes();
		return std::to_string(free / (1024 * 1024)) + " / " + std::to_string(total / (1024 * 1024)) + "MB, " + std::to_string((int)(100.0 * free / total)) + "%";
	}

	///////////////////////////////////////////////////////////////////////////
	// List directory contents
	// This function lists the contents of a directory up to a specified number of levels deep.
	void ListDir(fs::FS &fs, const char *dirname, uint8_t levels)
	{
		Serial.printf("Listing directory: %s\n", dirname);
		if (!_isMounted)
		{
			Serial.println("SD_MMC not mounted");
			return;
		}

		File root = fs.open(dirname);
		if (!root)
		{
			Serial.println("Failed to open directory");
			return;
		}
		if (!root.isDirectory())
		{
			Serial.println("Not a directory");
			return;
		}

		File file = root.openNextFile();
		while (file)
		{
			if (file.isDirectory())
			{
				Serial.print("  DIR : ");
				Serial.println(file.name());
				if (levels)
				{
					ListDir(fs, file.path(), levels - 1);
				}
			}
			else
			{
				Serial.print("  FILE: ");
				Serial.print(file.name());
				Serial.print("  SIZE: ");
				Serial.println(file.size());
			}
			file = root.openNextFile();
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Create a directory
	// This function creates a directory at the specified path.
	void CreateDir(fs::FS &fs, const char *path)
	{
		Serial.printf("Creating Dir: %s\n", path);
		if (!_isMounted)
		{
			Serial.println("SD_MMC not mounted");
			return;
		}
		if (!fs.mkdir(path))
			Serial.println("mkdir failed");
	}

	///////////////////////////////////////////////////////////////////////////
	// Remove a directory
	// This function removes a directory at the specified path.
	void RemoveDir(fs::FS &fs, const char *path)
	{
		Serial.printf("Removing Dir: %s\n", path);
		if (!_isMounted)
		{
			Serial.println("SD_MMC not mounted");
			return;
		}
		if (!fs.rmdir(path))
			Serial.println("rmdir failed");
	}

	///////////////////////////////////////////////////////////////////////////
	// Read a file
	// This function reads the contents of a file and prints it to the serial console.
	void ReadFile(fs::FS &fs, const char *path)
	{
		Serial.printf("Reading file: %s\n", path);
		if (!_isMounted)
		{
			Serial.println("SD_MMC not mounted");
			return;
		}
		File file = fs.open(path);
		if (!file)
		{
			Serial.println("Failed to open file for reading");
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
		Serial.printf("Writing file: %s\n", path);
		if (!_isMounted)
		{
			Serial.println("SD_MMC not mounted");
			return;
		}
		File file = fs.open(path, FILE_WRITE);
		if (!file)
		{
			Serial.println("Failed to open file for writing");
			return;
		}
		if (file.print(message))
		{
			Serial.println("File written");
		}
		else
		{
			Serial.println("Write failed");
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Append a message to a file
	// This function appends a message to a file at the specified path.
	void AppendFile(fs::FS &fs, const char *path, const char *message)
	{
		Serial.printf("Appending to file: %s\n", path);
		if (!_isMounted)
		{
			Serial.println("SD_MMC not mounted");
			return;
		}

		File file = fs.open(path, FILE_APPEND);
		if (!file)
		{
			Serial.println("Failed to open file for appending");
			return;
		}
		if (!file.print(message))
			Serial.println("Append failed");
	}

	///////////////////////////////////////////////////////////////////////////
	// Rename a file
	// This function renames a file from path1 to path2.
	void RenameFile(fs::FS &fs, const char *path1, const char *path2)
	{
		Serial.printf("Renaming file %s to %s\n", path1, path2);
		if (!_isMounted)
		{
			Serial.println("SD_MMC not mounted");
			return;
		}
		if (!fs.rename(path1, path2))
			Serial.println("Rename failed");
	}

	///////////////////////////////////////////////////////////////////////////
	// Delete a file
	// This function deletes a file at the specified path.
	void DeleteFile(fs::FS &fs, const char *path)
	{
		Serial.printf("Deleting file: %s\n", path);
		if (!_isMounted)
		{
			Serial.println("SD_MMC not mounted");
			return;
		}
		if (!fs.remove(path))
			Serial.println("Delete failed");
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
			Serial.printf("%u bytes read for %lu ms\n", flen, end);
			file.close();
		}
		else
		{
			Serial.println("Failed to open file for reading");
		}

		file = fs.open(path, FILE_WRITE);
		if (!file)
		{
			Serial.println("Failed to open file for writing");
			return;
		}

		size_t i;
		start = millis();
		for (i = 0; i < 2048; i++)
		{
			file.write(buf, 512);
		}
		end = millis() - start;
		Serial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
		file.close();
	}
};
