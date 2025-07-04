#pragma once
#include "HandyTime.h"

#define APP_VERSION "3.108.1058"

// Disables logging to serial
#define SERIAL_LOG

// Enables the LC29HDA code (Comment out for UM980 and UM982)
//#define IS_LC29HDA

#define  SERIAL_RX 17
#define SERIAL_TX 18

#define BACKLIGHT_CHANNEL 0 // Use channel 0 for backlight control

#define MAX_LOG_LENGTH (512)
#define MAX_LOG_SIZE (MAX_LOG_LENGTH * 80)
#define MAX_LOG_ROW_LENGTH (128 +24)

#define RTK_SERVERS 3

#define GPS_BUFFER_SIZE (16*1024)

// One 1 buttons on right, 3 Buttons on left
#define TFT_ROTATION  3

// WiFi access point password
#define AP_PASSWORD "John123456"
#define BASE_LOCATION_FILENAME "/BaseLocn.txt"
#define MDNS_HOST_FILENAME "/MDNS_HOST_Name.txt"
#define TIMEZONE_MINUTES "/TIMEZONE_MINUTES.txt"

extern HandyTime _handyTime;
extern std::string _mdnsHostName;