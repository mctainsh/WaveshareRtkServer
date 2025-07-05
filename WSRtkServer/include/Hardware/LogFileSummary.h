#pragma once
#include <string>
#include "FS.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief Details of a log file
struct LogFileSummary
{
	std::string Path;
	int Size;
	bool IsCurrentLog = false; // Indicate if this is the current log file

	LogFileSummary(fs::File f, bool isCurrentLog) : Path(f.path()), Size(f.size()), IsCurrentLog(isCurrentLog) {}

	bool operator<(const LogFileSummary &other) const
	{
		return strcmp(Path.c_str(), other.Path.c_str()) < 0;
	}
};