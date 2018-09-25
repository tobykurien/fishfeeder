#include <Arduino.h>
#include <EEPROM.h>
#include <RTClib.h>
#include "config.h"

#ifndef __LOGGER__
#define __LOGGER__

#define CHECK_BYTE  0xA2    // update this if data structures change

// Number of logs to keep for feedings and temperature
// NOTE: current code relies on byte wrap-around to implement
// a circular buffer, so it will need to change if this value
// is changed.
#define MAX_HISTORY 255 

struct Settings {
    uint8_t feedingScheme;
    uint8_t feedingAmount;
    uint8_t feedingDays; // each bit from lowest = day of week
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

        // logging
        void logFeeding(byte amount);
        void logTemperature();

        // settings
        void saveSettings();
        Settings* getSettings();
        
        // get data structures (logging, settings)
        DataStruct* getData();
        Feeding* getLastFeeding();
        Temperature* getLastTemperature();

        // time functions
        String getTime();
        String getTime(uint32_t time);
        uint32_t getCurrentTime();
        void setTime(DateTime time);

        // temperature
        float getTemperature();

    private:
        RTC_DS3231 rtc;
        DataStruct logData;
        void writeLogData();
};

#endif