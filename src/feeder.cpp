#include "feeder.h"

Feeder::Feeder(Logger* inLogger) {
    logger = inLogger;
}

void Feeder::start() {
    pinMode(SERVO_POWER, OUTPUT);
    digitalWrite(SERVO_POWER, LOW);
}

void Feeder::stop() {
}

// Check feeding schedule and feed accordingly
void Feeder::checkAndFeed() {
    Settings* s = logger->getSettings();

    DateTime now = DateTime(logger->getCurrentTime());
    // don't feed within the minimum feed times
    if (logger->getLastFeeding()->timestamp - logger->getCurrentTime() > MIN_FEEDING_TIMEOUT) {
        switch (s->feedingScheme) {
            case 0: // auto
                if (logger->getTemperature() >= 24.0) {
                    // feed thrice a day
                    if (now.hour() == 6) feedNow();
                    else if (now.hour() == 13) feedNow();
                    else if (now.hour() == 18) feedNow();
                } else if (logger->getTemperature() >= 20) {
                    // feed twice a day
                    if (now.hour() == 11) feedNow();
                    else if (now.hour() == 17) feedNow();
                } else if (logger->getTemperature() >= 16) {
                    // feed once a day
                    if (now.hour() == 17) feedNow();
                }
                break;
            case 1: // once a day
                if (now.hour() == 17) feedNow();
                break;
            case 2: // twice a day
                if (now.hour() == 11) feedNow();
                else if (now.hour() == 17) feedNow();
                break;
            case 3: // thrice a day
                if (now.hour() == 6) feedNow();
                else if (now.hour() == 13) feedNow();
                else if (now.hour() == 18) feedNow();
                break;
            default: // manual
                break;
        }
    }
}

int Feeder::feedNow() {
    return dumpFood();
}

int Feeder::dumpFood() {
    int amount = logger->getSettings()->feedingAmount;
    if (amount <= 0) {
        return ERR_AMOUNT_ZERO;
    }

    if (amount > 5) {
        amount = 5;
    }

    // safety: don't feed too often
    int last = logger->getData()->latestFeeding;
    Feeding lastFeeding = logger->getData()->feedings[last];
    if (logger->getCurrentTime() - lastFeeding.timestamp < MIN_FEEDING_TIMEOUT) {
        return ERR_TOO_SOON;
    }

    // don't feed at night
    DateTime now = DateTime(logger->getCurrentTime());
    if (now.hour() < 5 || now.hour() > 20) {
        return ERR_NIGHT_FEED;
    }

    logger->logFeeding(amount);
    Serial.println("Dumping food");

    // start up servo
    digitalWrite(SERVO_POWER, HIGH);
    delay(500);
    servo.attach(SERVO);

    for (int i=0; i < amount; i++) {
        // turn the feeder
        servo.write(0);
        delay(500);
        servo.write(90);
        delay(500);
        servo.write(180);
        delay(500);
        servo.write(90);
        delay(500);
        servo.write(0);
        delay(500);
    }

    // shutdown the servo
    servo.detach();
    digitalWrite(SERVO_POWER, LOW);

    return amount;
}
