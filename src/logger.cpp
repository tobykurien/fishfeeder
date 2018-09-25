#include "logger.h"

Logger::Logger() {
}

void Logger::start() {
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC, stopping.");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, lets set the time!");
        // following line sets the RTC to the date & time this sketch was compiled
        //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        //rtc.adjust(DateTime(2018, 8, 28, 16, 16, 0));
    }

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

Settings* Logger::getSettings() {
    return &(logData.settings);
}

void Logger::stop() {
    EEPROM.end();
}

void Logger::logFeeding(byte amount) {
    Feeding f;
    f.temperature = getTemperature();
    f.timestamp = getCurrentTime();
    f.amount = amount;
    logData.feedings[++logData.latestFeeding] = f;
    writeLogData();
}

void Logger::logTemperature() {
    float diff = logData.temperatures[logData.latestTemperature]
                    .temperature - getTemperature();

    if (diff < -0.5 || diff > 0.5) {
        Temperature t;
        t.temperature = getTemperature();
        t.timestamp = getCurrentTime();
        logData.temperatures[++logData.latestTemperature] = t;
        writeLogData();
    }
}

DataStruct* Logger::getData() {
    return &logData;
}

String Logger::getTime() {
    return getTime(rtc.now().unixtime());
}

String Logger::getTime(uint32_t time) {
    DateTime now = DateTime(time);
    return String(now.year()) + '/' +
        String(now.month()) + '/' +
        String(now.day()) + ' ' +
        String(now.hour()) + ':' +
        String(now.minute()) + ':' +
        String(now.second());
}

uint32_t Logger::getCurrentTime() {
    return rtc.now().unixtime();
}

void Logger::setTime(DateTime time) {
    rtc.adjust(time);
}

float Logger::getTemperature() {
    digitalWrite(TEMP_POWER, HIGH);
    delay(10);
    int t = analogRead(TEMP_SENS);
    digitalWrite(TEMP_POWER, LOW);

    // calibrated to degrees celcius with thermometer
    int reading = map(t*10, 4970, 7390, 270, 50);
    return reading / 10.0;
}

