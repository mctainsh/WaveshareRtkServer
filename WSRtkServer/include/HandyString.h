#pragma once

#include <WiFi.h>
#include <string>
#include <vector>

#define KB 1024
#define MEGAB 1024*1024

template<typename... Args>
std::string StringPrintf(const std::string& format, Args... args);

bool StartsWith(const std::string& fullString, const std::string& startString);
bool StartsWith(const char* szA, const char* szB);
std::string ToThousands(int64_t number);
std::string HexDump(const unsigned char* data, int len);
std::string HexAsciDump( const unsigned char *data, int len );
std::vector<std::string> Split(const std::string& s, const std::string delimiter);
void FormatNumber(int number, int width, std::string& result);
bool EndsWith(const std::string& fullString, const std::string& ending);
bool IsValidHex(const std::string& str);
bool IsValidDouble(const char* str, double *pVal);
const char* WifiStatus(wl_status_t status);
const char *WiFiModeText(wifi_mode_t mode);
std::string ReplaceNewlineWithTab(const std::string& input);
std::string Replace(const std::string &input, const std::string &search, const std::string &replace);
void RemoveLastLfCr(std::string &str);
void ReplaceCrLfEncode(std::string &str);
std::string Trim(const std::string& str);
std::string ToLower(const std::string& str);
std::string MakeKbPercent(u64_t usedBytes, u64_t totalBytes, u64_t divisor = KB );


#include "HandyString.tpp"
