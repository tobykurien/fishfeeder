#include <Arduino.h>
#include <EEPROM.h>
#include <RTClib.h>
#include "config.h"

#ifndef __LOGGER__
#define __LOGGER__

#define CHECK_BYTE  0xA1

// Number of logs to keep for feedings and temperature
// NOTE: current code relies on byte wrap-around to implement
// a circular buffer, so it will need to change if this value
// is changed.
#define MAX_HISTORY 255 

struct Settings {
    uint8_t feedingScheme;
    uint8_t feedingAmount;
};

struct Feeding {
    uint32_t timestamp;
    uint8_t amount;
    float temperature;
};

struct Temperature {
    uint32_t timestamp;
    float temperature;
};

// EEPROM data structure for all saved data
struct DataStruct {
    Settings settings;
    uint8_t latestFeeding;
    Feeding feedings[MAX_HISTORY];
    uint8_t latestTemperature;
    Temperature temperatures[MAX_HISTORY];
};

class Logger {
    public:
        Logger();
        void start();
        void stop();

        void logFeeding(byte amount);
        void logTemperature();

        void saveSettings(byte feedingScheme, byte feedingAmount);
        Settings getSettings();
        
        DataStruct* getData();

        // time functions
        String getTime();
        String getTime(uint32_t time);
        uint32_t getCurrentTime();
        void setTime(DateTime time);

        float getTemperature();

    private:
        RTC_DS3231 rtc;
        DataStruct logData;
        void writeLogData();
};

#endif