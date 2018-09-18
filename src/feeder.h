#include <Arduino.h>
#include <Servo.h>
#include "config.h"

#ifndef __FEEDER__
#define __FEEDER__

class Feeder {
    public:
        Feeder();
        void start();
        void stop();

        int checkAndFeed();
        void feedNow();

    private:
        Servo servo;
        void dumpFood();
};

#endif