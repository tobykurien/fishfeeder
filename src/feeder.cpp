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

int Feeder::checkAndFeed() {
    // TODO - check feeding schedule and feed accordingly
    return 0;
}

void Feeder::feedNow() {
    dumpFood();
}

void Feeder::dumpFood() {
    int amount = logger->getSettings()->feedingAmount;
    if (amount <= 0) {
        return;
    }

    if (amount > 5) {
        amount = 5;
    }

    // safety: don't feed too often
    int last = logger->getData()->latestFeeding;
    Feeding lastFeeding = logger->getData()->feedings[last];
    if (logger->getCurrentTime() - lastFeeding.timestamp < MIN_FEEDING_TIMEOUT) {
        Serial.println("Too soon");
        return;
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
}
