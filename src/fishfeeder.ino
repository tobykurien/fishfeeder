#include <Arduino.h>
#include <Servo.h>

#define SERVO   3
#define LED     12

unsigned long millisHour = 3600000; // adjust for clock drift. 3600000 in a perfect world.
unsigned long startMillis = 0;

Servo servo;

void setup() {
    Serial.begin(9600);
    Serial.println("Starting fish feeder");

    servo.attach(SERVO);
    servo.write(0);
    pinMode(LED, OUTPUT);

    // wait till start time
    delayHours(5, millis());
}

void loop() {
    // 9 am
    startMillis = millis();
    dumpFood();
    delayHours(7, startMillis);

    // 4 pm
    startMillis = millis();
    dumpFood();
    delayHours(17, startMillis);
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
    
    delay(5000);

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

void delayHours(unsigned int hours, unsigned long start) {
    Serial.print("Delaying for hours ");
    Serial.println(hours);

    unsigned long totalMillis = hours * millisHour;

    for (int hour=hours; hour > 0; hour--) {
        Serial.print("Starting hour ");
        Serial.println(hour);

        while ((millis() - start) < millisHour) {
            //Serial.println(millis() - start);

            if (hour > 1) {
                delay(1000 * hour);
            } else {
                if ((millis() - start) < 1800000) {
                    // more than 30 mins to go
                    delay(750);
                } else if ((millis() - start) < 3000000) {
                    // more than 10 minutes to go
                    delay(500);
                } else if ((millis() - start) < 3540000) {
                    // more than 1 minute to go
                    delay(250);
                } else {
                    delay(100);
                }
            }

            digitalWrite(LED, HIGH);
            delay(100);
            digitalWrite(LED, LOW);
        }
    } 
}

