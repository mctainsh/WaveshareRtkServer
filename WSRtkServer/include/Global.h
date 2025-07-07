#pragma once

#define I2C_SDA 8
#define I2C_SCL 7

#define APP_VERSION "3.108.1081"

// Disables logging to serial
#define SERIAL_LOG

// Enables the LC29HDA code (Comment out for UM980 and UM982)
// #define IS_LC29HDA

#define SERIAL_RX 18 // TX on UM980/982
#define SERIAL_TX 17 // RX on UM980/982

#define BACKLIGHT_CHANNEL 0 // Use channel 0 for backlight dimmer control

#define MAX_LOG_LENGTH (512)
#define MAX_LOG_SIZE (MAX_LOG_LENGTH * 80)
#define MAX_LOG_ROW_LENGTH (128 + 24)

#define RTK_SERVERS 3

#define GPS_BUFFER_SIZE (16 * 1024)

// File system definitions
#define BOOT_LOG_FILENAME "/bootlog.txt"
#define BOOT_LOG_MAX_LENGTH 100
#define LOG_FILE_PREFIX "/logs/"

// WiFi access point password
#define AP_PASSWORD "John123456"
#define BASE_LOCATION_FILENAME "/BaseLocn.txt"
#define MDNS_HOST_FILENAME "/MDNS_HOST_Name.txt"
#define TIMEZONE_MINUTES "/TIMEZONE_MINUTES.txt"

#include "HandyTime.h"
extern HandyTime _handyTime;
extern std::string _mdnsHostName;