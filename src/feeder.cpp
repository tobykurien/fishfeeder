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
    // TODO - check last feed time > 1 hour
    dumpFood();
}

void Feeder::dumpFood() {
    logger->logFeeding(1);
    Serial.println("Dumping food");

    // start up servo
    digitalWrite(SERVO_POWER, HIGH);
    delay(500);
    servo.attach(SERVO);

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

    // shutdown the servo
    servo.detach();
    digitalWrite(SERVO_POWER, LOW);
}
