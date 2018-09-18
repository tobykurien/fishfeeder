#include "logger.h"

Logger::Logger() {
}

void Logger::start() {
    EEPROM.begin(sizeof(DataStruct));
}

void Logger::stop() {
    EEPROM.end();
}

void Logger::logFeeding(uint32_t timestamp, byte amount, float temperature) {

}

void Logger::logTemperature(uint32_t timestamp, float temperature) {
    
}