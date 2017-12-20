#include <Arduino.h>
#include <Servo.h>

#define SERVO   3
#define LED     12

Servo servo;

void setup() {
    Serial.begin(9600);
    Serial.println("Starting fish feeder");

    servo.attach(SERVO);
    servo.write(0);
    pinMode(LED, OUTPUT);

    // Avoid multiple food dumpings on startup
    digitalWrite(LED, HIGH);
    delay(2000);
    digitalWrite(LED, LOW);
}

void loop() {
    // test
    delayHours(1);

    // 7 am
    dumpFood();
    delayHours(6);

    // 1 pm
    dumpFood();
    delayHours(5);

    // 6 pm
    dumpFood();
    delayHours(13);
}

void dumpFood() {
    Serial.println("Dumping food");
    servo.write(0);
    delay(500);
    servo.write(90);
    delay(500);
    servo.write(180);
    delay(500);
    servo.write(90);
    delay(500);
    servo.write(0);
}

void delayHours(unsigned int hours) {
    Serial.print("Delaying for hours ");
    Serial.println(hours);

    unsigned long millisHour = 3600000;
    unsigned long totalMillis = hours * millisHour;
    Serial.print("In milliseconds: ");
    Serial.println(totalMillis);

    for (int hour=hours; hour > 0; hour--) {
        Serial.print("Starting hour ");
        Serial.println(hour);
        unsigned long start = millis();

        while ((millis() - start) < millisHour) {
            //Serial.println(millis() - start);

            if (hour > 1) {
                delay(1000 * hour);
            } else {
                if ((millis() - start) < 1800000) {
                    // more than 30 mins to go
                    Serial.print("+");
                    delay(750);
                } else if ((millis() - start) < 3000000) {
                    // more than 10 minutes to go
                    Serial.print("-");
                    delay(500);
                } else if ((millis() - start) < 3540000) {
                    // more than 1 minute to go
                    Serial.print("*");
                    delay(250);
                } else {
                    Serial.print(".");
                    delay(100);
                }
            }

            digitalWrite(LED, HIGH);
            delay(100);
            digitalWrite(LED, LOW);
        }
    } 
}

