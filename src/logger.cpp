#include "logger.h"

Logger::Logger() {
}

void Logger::start() {
    EEPROM.begin(sizeof(DataStruct) + 1);
    
    Serial.print("Data needed for EERPROM: ");
    Serial.println(sizeof(DataStruct));

    if (EEPROM.read(0) == CHECK_BYTE) {
        // load previous data
        for(size_t i = 1; i < sizeof(DataStruct); i++) {
            char data = EEPROM.read(i);
            ((char *)&logData)[i] = data;
        }

        Serial.println("Loaded log data:");
        Serial.println(logData.latestTemperature);
        Serial.println(logData.temperatures[1].temperature);
     }
}

void Logger::writeLogData() {
    EEPROM.write(0, CHECK_BYTE);
    for(size_t i = 1; i < sizeof(DataStruct); i++) {
        char data = ((char *)&logData)[i];
        EEPROM.write(i, data);
    }
    EEPROM.commit();
    Serial.println("Wrote log data to EEPROM");    
    Serial.println(logData.latestTemperature);
    Serial.println(logData.temperatures[1].temperature);
}

void Logger::stop() {
    EEPROM.end();
}

void Logger::logFeeding(uint32_t timestamp, byte amount, float temperature) {

}

void Logger::logTemperature(uint32_t timestamp, float temperature) {
    float diff = logData.temperatures[logData.latestTemperature].temperature - temperature;

    if (diff < -0.5 || diff > 0.5) {
        Temperature t;
        t.temperature = temperature;
        t.timestamp = timestamp;
        logData.temperatures[++logData.latestTemperature] = t;
        writeLogData();
    }
}

DataStruct Logger::getData() {
    return logData;
}
