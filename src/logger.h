#include <Arduino.h>
#include <EEPROM.h>

#ifndef __LOGGER__
#define __LOGGER__

#define CHECK_BYTE  0xA1
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

        void logFeeding(uint32_t timestamp, byte amount, float temperature);
        void logTemperature(uint32_t timestamp, float temperature);

        void saveSettings(byte feedingScheme, byte feedingAmount);
        Settings getSettings();
        
        DataStruct getData();

    private:
        DataStruct logData;
        void writeLogData();
};

#endif