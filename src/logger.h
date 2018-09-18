#include <Arduino.h>
#include <EEPROM.h>

#ifndef __LOGGER__
#define __LOGGER__

struct Feeding {
    uint32_t timestamp;
    byte amount;
    int temperature;
};

struct Temperature {
    uint32_t timestamp;
    int temperature;
};

struct DataStruct {
    byte latestFeeding;
    Feeding feedings[255];
    byte latestTemperature;
    Temperature temperatures[255];
};

class Logger {
    public:
        Logger();
        void start();
        void stop();

        void logFeeding(uint32_t timestamp, byte amount, float temperature);
        void logTemperature(uint32_t timestamp, float temperature);
        
        Feeding* getFeedings(int batch);
        Temperature* getTemperatures(int batch);

    private:
};

#endif